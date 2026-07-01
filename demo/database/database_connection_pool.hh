#ifndef UGREEN_CORE_DATABASE_DATABASE_HH_
#define UGREEN_CORE_DATABASE_DATABASE_HH_

/// @file database_connection_pool.hh
/// @author UGreen NAS Team
/// @brief 数据库抽象接口定义
/// @date 2026-01-20
/// @copyright Copyright (c) 2026

#include <memory>

#include <ugreen/ugos/core/database/database_type.hh>
#include <ugreen/ugos/core/database/database_connection.hh>

namespace ugreen::core::database {

/// @brief 数据库抽象接口（基于 SOCI）
/// @details 提供数据库连接池管理和 ORM 风格的 CRUD 操作接口
/// 支持 PostgreSQL 和 SQLite3 两种数据库类型
class IDatabaseConnectionPool {
 public:
  virtual ~IDatabaseConnectionPool() = default;

  /// @brief 打开 PostgreSQL 数据库连接池
  /// @param config PostgreSQL 数据库配置
  /// @return 成功返回 true，失败返回 false
  virtual bool Open(const PostgresqlConfig& config) = 0;

  /// @brief 打开 SQLite 数据库连接池
  /// @param config SQLite 数据库配置
  /// @return 成功返回 true，失败返回 false
  virtual bool Open(const SQLiteConfig& config) = 0;

  /// @brief 检查数据库是否已打开
  /// @return 已打开返回 true，否则返回 false
  virtual bool IsOpen() const = 0;

  /// @brief 关闭数据库连接池（noexcept）
  /// @details 释放占用的连接池槽位，但不销毁全局连接池
  virtual void Close() noexcept = 0;

  /// @brief 获取数据库连接（RAII 管理）
  /// @param timeout_ms 超时时间（毫秒），0 表示不阻塞立即返回
  /// @return 成功返回连接对象的 unique_ptr，失败或超时返回 nullptr
  /// @details 返回的连接对象在析构时会自动归还给连接池
  /// @note DML 操作（Insert、Update、Delete、Select、Count）应在连接对象上执行
  virtual std::unique_ptr<IDatabaseConnection> GetConnection(unsigned int timeout_ms = 0) = 0;
};

/// @brief 创建并打开数据库连接池（模板函数，支持 PostgreSQL 和 SQLite）
/// @tparam ConfigType 配置类型（PostgresqlConfig 或 SQLiteConfig）
/// @param config 数据库配置
/// @return 成功返回已打开的数据库实例，失败返回 nullptr
/// @note 此函数合并了创建和打开数据库的操作，推荐使用此函数
template <class ConfigType>
std::unique_ptr<IDatabaseConnectionPool> create_database_connection_pool(const ConfigType& config);

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_DATABASE_HH_
