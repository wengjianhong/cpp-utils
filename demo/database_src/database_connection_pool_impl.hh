#ifndef UGREEN_CORE_DATABASE_DATABASE_IMPL_HH_
#define UGREEN_CORE_DATABASE_DATABASE_IMPL_HH_

#include <memory>
#include <variant>

#include <ugreen/ugos/core/database/database_type.hh>
#include <ugreen/ugos/core/database/database_connection.hh>
#include <ugreen/ugos/core/database/database_connection_pool.hh>

namespace ugreen::core::database {

// 前向声明
struct IConnectionPool;
template <DatabaseType Type>
struct ConnectionPool;

/** @brief 基于 SOCI 的数据库连接池实现（每个实例拥有独立的连接池） */
class DatabaseImpl : public IDatabaseConnectionPool {
 public:
  DatabaseImpl() = default;
  virtual ~DatabaseImpl() noexcept override;

  /** @brief 打开 PostgreSQL 数据库（创建独立的连接池） */
  bool Open(const PostgresqlConfig& config) override;

  /** @brief 打开 SQLite 数据库（创建独立的连接池） */
  bool Open(const SQLiteConfig& config) override;

  /** @brief 检查是否已打开 */
  bool IsOpen() const override;

  /** @brief 关闭数据库（销毁连接池） */
  void Close() noexcept override;

  /** @brief 获取数据库连接（RAII 管理） */
  std::unique_ptr<IDatabaseConnection> GetConnection(unsigned int timeout_ms = 0) override;

 private:
  bool open_ = false;                                   /**< 是否已打开 */
  std::variant<PostgresqlConfig, SQLiteConfig> config_; /**< 数据库配置 */
  std::variant<std::unique_ptr<ConnectionPool<DatabaseType::POSTGRESQL>>,
               std::unique_ptr<ConnectionPool<DatabaseType::SQLITE3>>>
    pool_; /**< 连接池（类型擦除，消除类型判断） */

  // 内部辅助函数：获取 IConnectionPool 指针
  IConnectionPool* GetPoolPtr() const;
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_DATABASE_IMPL_HH_
