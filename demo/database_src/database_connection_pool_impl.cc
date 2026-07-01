// system headers
#include <exception>
#include <memory>
#include <variant>

// third-party headers
#include <soci/soci.h>

// ugreen headers
#include <ugreen/ugos/core/log/logging.hh>
#include <ugreen/ugos/core/database/database_connection_pool.hh>

// relative headers
#include "connection_pool.hh"
#include "database_connection_impl.hh"
#include "database_connection_pool_impl.hh"

namespace ugreen::core::database {

// ============================================================================
// 数据库实现：DatabaseImpl
// ============================================================================

DatabaseImpl::~DatabaseImpl() noexcept {
  Close();
}

bool DatabaseImpl::Open(const PostgresqlConfig& config) {
  if (open_) {
    Close();
  }

  config_ = config;
  auto pool = std::make_unique<ConnectionPool<DatabaseType::POSTGRESQL>>();
  if (!pool->Initialize(config, config.soci_options)) {
    LOG(ERROR) << "Failed to initialize PostgreSQL connection pool";
    return false;
  }
  pool_ = std::move(pool);
  open_ = true;
  return true;
}

bool DatabaseImpl::Open(const SQLiteConfig& config) {
  if (open_) {
    Close();
  }

  config_ = config;
  auto pool = std::make_unique<ConnectionPool<DatabaseType::SQLITE3>>();
  if (!pool->Initialize(config, config.soci_options)) {
    LOG(ERROR) << "Failed to initialize SQLite connection pool";
    return false;
  }
  pool_ = std::move(pool);
  open_ = true;
  return true;
}

bool DatabaseImpl::IsOpen() const {
  return open_;
}

void DatabaseImpl::Close() noexcept {
  if (!open_) {
    return;
  }
  // 关闭连接池：标记为未初始化，阻止新的连接请求
  // 不立即销毁连接池，让已存在的连接对象在析构时能够安全归还连接
  std::visit(
    [](auto& pool_ptr) {
      if (pool_ptr) {
        pool_ptr->Shutdown();
      }
    },
    pool_);
  open_ = false;
}

IConnectionPool* DatabaseImpl::GetPoolPtr() const {
  if (!open_) {
    return nullptr;
  }
  return std::visit([](const auto& pool_ptr) -> IConnectionPool* { return pool_ptr ? pool_ptr.get() : nullptr; },
                    pool_);
}

std::unique_ptr<IDatabaseConnection> DatabaseImpl::GetConnection(unsigned int timeout_ms) {
  IConnectionPool* pool = GetPoolPtr();
  if (!pool) {
    return nullptr;
  }

  soci::connection_pool* soci_pool = pool->GetPool();
  if (!soci_pool || !pool->IsInitialized()) {
    return nullptr;
  }

  std::size_t pos = 0;
  if (!soci_pool->try_lease(pos, timeout_ms)) {
    return nullptr;
  }

  auto conn = std::make_unique<DatabaseConnectionImpl>(soci_pool, pos, *pool);

  try {
    soci::session& session = soci_pool->at(pos);
    const soci::connection_parameters* params = pool->GetParams();

    // 检查连接是否需要重连：只依赖 is_connected() 判断，不执行 ping test
    bool needs_reconnect = !session.is_connected();

    // 执行重连
    if (needs_reconnect && params) {
      try {
        session.close();
      } catch (const soci::soci_error& e) {
        LOG(WARNING) << "Ignoring error during session close: " << e.what();
      }
      session.open(*params);
    }

    return conn;
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Database GetConnection error: " << ex.what();
    return nullptr;
  }
}

// ============================================================================
// 全局函数：数据库实例创建和管理
// ============================================================================

namespace {
// 内部函数：创建数据库实例（不对外暴露）
std::unique_ptr<IDatabaseConnectionPool> create_database_instance() {
  return std::make_unique<DatabaseImpl>();
}
}  // namespace

template <class ConfigType>
std::unique_ptr<IDatabaseConnectionPool> create_database_connection_pool(const ConfigType& config) {
  auto db = create_database_instance();
  if (!db->Open(config)) {
    return nullptr;
  }
  return db;
}

// 显式实例化模板函数
template std::unique_ptr<IDatabaseConnectionPool> create_database_connection_pool<PostgresqlConfig>(
  const PostgresqlConfig& config);
template std::unique_ptr<IDatabaseConnectionPool> create_database_connection_pool<SQLiteConfig>(
  const SQLiteConfig& config);

}  // namespace ugreen::core::database
