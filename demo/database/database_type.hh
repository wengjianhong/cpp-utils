#ifndef UGREEN_CORE_DATABASE_DATABASE_TYPES_HH_
#define UGREEN_CORE_DATABASE_DATABASE_TYPES_HH_

/// @file database_type.hh
/// @author UGreen NAS Team
/// @brief 数据库配置和枚举类型定义
/// @date 2026-01-20
/// @copyright Copyright (c) 2026

#include <map>
#include <chrono>
#include <cstddef>
#include <string>

/// @brief 数据库相关功能模块
namespace ugreen::core::database {

// ============================================================================
// 数据库类型枚举（内部使用）
// ============================================================================
enum class DatabaseType {  /// 数据库类型枚举（内部使用）
  POSTGRESQL = 0,          /// PostgreSQL
  SQLITE3,                 /// SQLite3
};

// ============================================================================
// 数据库配置（PostgreSQL 和 SQLite 完全独立）
// ============================================================================
enum class PostgresqlConnectionType {  /// PostgreSQL 连接类型枚举
  TCP = 0,                             /// TCP 连接
  UNIX,                                /// Unix socket 连接
};

/// @brief PostgreSQL 数据库配置（完全独立，不包含 SQLite 相关代码）
/// @details 使用 PostgreSQL 的用户只需使用此配置类，不会看到 SQLite 相关代码
struct PostgresqlConfig {
  int port = 5432;                                                /// 端口
  std::string host = "127.0.0.1";                                 /// 服务器地址
  std::string user = "ugreen";                                    /// 用户名
  std::string password = "";                                      /// 密码（空表示 trust 认证）
  std::string database_name = "nasdb";                            /// 数据库名
  std::string socket_path = "/var/ugreen/postgresql";             /// Unix socket 路径
  std::string ssl_mode = "disable";                               /// SSL 模式
  PostgresqlConnectionType type = PostgresqlConnectionType::TCP;  /// 连接类型

  int lease_timeout_ms = 0;                             /// 获取连接超时（ms，<=0=不阻塞）
  std::size_t max_connection_size = 10;                 /// 最大连接数
  std::chrono::seconds max_idle_time{60};               /// 连接最大空闲时间（s，0=不回收）
  std::chrono::seconds max_lifetime{180};               /// 连接最大生命周期（s，0=不限制）
  std::chrono::milliseconds connect_timeout{0};         /// 连接超时（ms，0=默认）
  std::chrono::milliseconds slow_query_threshold{300};  /// 慢查询阈值（ms）
  std::map<std::string, std::string> soci_options = {{"reconnect", "true"}};  /// SOCI 选项（如 {"reconnect", "true"}）
};

/// @brief SQLite 数据库配置（完全独立，不包含 PostgreSQL 相关代码）
/// @details 使用 SQLite 的用户只需使用此配置类，不会看到 PostgreSQL 相关代码
struct SQLiteConfig {
  int lease_timeout_ms = 0;                /// 获取连接超时（ms，<=0=不阻塞）
  std::size_t max_connection_size = 10;    /// 最大连接数
  std::string database_path = ":memory:";  /// 数据库路径（":memory:" 表示内存数据库）
  std::map<std::string, std::string> soci_options = {{"reconnect", "true"}};  /// SOCI 选项（如 {"reconnect", "true"}）
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_DATABASE_TYPES_HH_
