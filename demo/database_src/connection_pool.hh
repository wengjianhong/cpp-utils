#ifndef UGREEN_CORE_DATABASE_CONNECTION_POOL_HH_
#define UGREEN_CORE_DATABASE_CONNECTION_POOL_HH_

// system headers
#include <atomic>
#include <cstring>
#include <map>
#include <memory>
#include <sstream>
#include <string>

// third-party headers
#include <soci/connection-pool.h>
#include <soci/session.h>
#include <soci/statement.h>

// PostgreSQL headers
#include <libpq-fe.h>
#include <soci/postgresql/soci-postgresql.h>

// SQLite headers
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>

// ugreen headers
#include <ugreen/ugos/core/common/exception.hh>
#include <ugreen/ugos/core/database/database_type.hh>
#include <ugreen/ugos/core/log/logging.hh>

namespace ugreen::core::database {

// ============================================================================
// 配置相关：数据库类型 Traits（用于提取数据库特定逻辑）
// ============================================================================

namespace {

/// 数据库类型 traits 模板（用于提取数据库特定逻辑）
template <DatabaseType Type>
struct DatabaseTraits;

/// PostgreSQL traits 特化
template <>
struct DatabaseTraits<DatabaseType::POSTGRESQL> {
  using ConfigType = PostgresqlConfig;
  static constexpr const char* BackendName() { return "postgresql"; }

  static std::string BuildConnectString(const PostgresqlConfig& cfg) {
    std::ostringstream ss;
    if (cfg.type == PostgresqlConnectionType::UNIX) {
      ss << "host=" << cfg.socket_path << " port=" << cfg.port << " ";
    } else {
      ss << "host=" << cfg.host << " port=" << cfg.port << " ";
    }
    if (!cfg.database_name.empty()) {
      ss << "dbname=" << cfg.database_name << " ";
    }
    if (!cfg.user.empty()) {
      ss << "user=" << cfg.user << " ";
    }
    if (!cfg.password.empty()) {
      ss << "password=" << cfg.password << " ";
    }
    if (!cfg.ssl_mode.empty()) {
      ss << "sslmode=" << cfg.ssl_mode << " ";
    }

    if (cfg.connect_timeout.count() > 0) {
      auto seconds = (cfg.connect_timeout.count() + 999) / 1000;
      ss << "connect_timeout=" << seconds << " ";
    }
    return ss.str();
  }

  static std::size_t GetPoolSize(const PostgresqlConfig& cfg) {
    return (cfg.max_connection_size > 0) ? cfg.max_connection_size : 1;
  }

  static bool ShouldLogSlowQuery(const PostgresqlConfig& cfg) { return cfg.slow_query_threshold.count() > 0; }

  static int64_t GetSlowQueryThreshold(const PostgresqlConfig& cfg) { return cfg.slow_query_threshold.count(); }

  static int64_t GetAffectedRows(soci::statement& st, soci::session& /*session*/) { return st.get_affected_rows(); }
};

/// SQLite traits 特化
template <>
struct DatabaseTraits<DatabaseType::SQLITE3> {
  using ConfigType = SQLiteConfig;
  static constexpr const char* BackendName() { return "sqlite3"; }

  static std::string BuildConnectString(const SQLiteConfig& cfg) { return cfg.database_path; }

  static std::size_t GetPoolSize(const SQLiteConfig& cfg) {
    return (cfg.max_connection_size > 0) ? cfg.max_connection_size : 1;
  }

  static bool ShouldLogSlowQuery(const SQLiteConfig&) {
    return false;  // SQLite 不支持慢查询日志
  }

  static int64_t GetSlowQueryThreshold(const SQLiteConfig&) { return 0; }

  static int64_t GetAffectedRows(soci::statement& st, soci::session& session) {
    int64_t affected = st.get_affected_rows();
    if (affected >= 0) {
      return affected;
    }
    // SQLite 特定方法
    int64_t changes = 0;
    session << "SELECT changes()", soci::into(changes);
    return changes;
  }
};

}  // namespace

// ============================================================================
// 连接池相关：连接池管理
// ============================================================================

/// 连接池基类接口（类型擦除，用于 DatabaseConnectionImpl 和 DatabaseImpl，消除类型判断）
struct IConnectionPool {
  virtual ~IConnectionPool() noexcept = default;
  virtual bool IsInitialized() const = 0;
  virtual soci::connection_pool* GetPool() const = 0;
  virtual const soci::connection_parameters* GetParams() const = 0;
  virtual int64_t GetAffectedRows(soci::statement& st, soci::session& session) const = 0;
  virtual bool ShouldLogSlowQuery() const = 0;
  virtual int64_t GetSlowQueryThreshold() const = 0;
  virtual std::size_t GetPoolSize() const = 0;
  virtual ::ugreen::core::database::DatabaseType GetDatabaseType() const = 0;
  virtual std::string EscapeIdentifier(const std::string& identifier, soci::session* session) const = 0;
};

/// 连接池管理器（模板化，按数据库类型独立）
template <DatabaseType Type>
struct ConnectionPool : public IConnectionPool {
  using Traits = DatabaseTraits<Type>;
  using ConfigType = typename Traits::ConfigType;

  std::atomic<bool> Initialized_{false};
  ConfigType Config_;
  std::map<std::string, std::string> SociOptions;
  std::unique_ptr<soci::connection_parameters> Params_;
  std::unique_ptr<soci::connection_pool> Pool_;
  std::size_t PoolSize_ = 0;

  /// 初始化连接池
  bool Initialize(const ConfigType& config, const std::map<std::string, std::string>& options) {
    Config_ = config;
    SociOptions = options;
    try {
      std::string connect_string = Traits::BuildConnectString(Config_);

      Params_ = std::make_unique<soci::connection_parameters>(Traits::BackendName(), connect_string);
      for (const auto& kv : SociOptions) {
        Params_->set_option(kv.first.c_str(), kv.second);
      }

      PoolSize_ = Traits::GetPoolSize(Config_);
      Pool_ = std::make_unique<soci::connection_pool>(PoolSize_);
      Initialized_.store(true);

      return true;
    } catch (const soci::soci_error& ex) {
      LOG(ERROR) << "Connection pool initialization failed: " << ex.what();
      return false;
    }
  }

  bool IsInitialized() const override { return Initialized_.load(); }

  soci::connection_pool* GetPool() const override { return Pool_.get(); }

  const soci::connection_parameters* GetParams() const override { return Params_.get(); }

  ::ugreen::core::database::DatabaseType GetDatabaseType() const override { return Type; }

  std::string EscapeIdentifier(const std::string& identifier, soci::session* session) const override {
    if (!session) {
      UG_THROW_BASE("EscapeIdentifier requires a valid session, must be called within a connection context");
    }

    if constexpr (Type == DatabaseType::POSTGRESQL) {
      std::string backend_name = session->get_backend_name();
      if (backend_name != "postgresql") {
        UG_THROW_BASE("Expected PostgreSQL backend, but got: " + backend_name);
      }

      void* backend_ptr = session->get_backend();
      if (!backend_ptr) {
        UG_THROW_BASE("Failed to get PostgreSQL backend from session");
      }

      // SOCI PostgreSQL backend 的结构可能因版本而异
      struct pg_session_backend {
        PGconn* conn_;
      };
      pg_session_backend* pg_backend = static_cast<pg_session_backend*>(backend_ptr);
      if (!pg_backend || !pg_backend->conn_) {
        UG_THROW_BASE("PostgreSQL connection is null or invalid");
      }

      char* escaped = PQescapeIdentifier(pg_backend->conn_, identifier.c_str(), identifier.length());
      if (!escaped) {
        std::string error_msg = PQerrorMessage(pg_backend->conn_);
        UG_THROW_BASE("PQescapeIdentifier failed: " + error_msg);
      }

      std::string result(escaped);
      PQfreemem(escaped);
      return result;
    } else {
      char* escaped = sqlite3_mprintf("\"%w\"", identifier.c_str());
      if (!escaped) {
        UG_THROW_BASE("sqlite3_mprintf failed: out of memory");
      }

      std::string result(escaped);
      sqlite3_free(escaped);
      return result;
    }
  }

  int64_t GetAffectedRows(soci::statement& st, soci::session& session) const override {
    return Traits::GetAffectedRows(st, session);
  }

  bool ShouldLogSlowQuery() const override { return Traits::ShouldLogSlowQuery(Config_); }

  int64_t GetSlowQueryThreshold() const override { return Traits::GetSlowQueryThreshold(Config_); }

  std::size_t GetPoolSize() const override { return PoolSize_; }

  /// 关闭连接池
  void Shutdown() noexcept {
    if (!Initialized_.load()) {
      return;
    }
    // 标记为未初始化，阻止新的连接请求
    // 不立即销毁 Pool_，让已存在的连接对象在析构时能够安全归还连接
    Initialized_.store(false);
  }
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_CONNECTION_POOL_HH_
