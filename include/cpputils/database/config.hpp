/// @file      config.hpp
/// @brief     数据库连接配置与 conn_string 构建
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_CONFIG_HPP_
#define CPP_UTILS_DATABASE_CONFIG_HPP_

#include <cpputils/database/database_types.hpp>

#include <cstddef>
#include <ctime>
#include <map>
#include <string>

namespace cpputils::database {

/// @brief PostgreSQL 连接方式
enum class ConnectionType {
  ///< TCP
  kTcp = 0,
  ///< Unix socket
  kUnix = 1,
};

/// @brief SQLite 连接配置
struct SqliteConfig {
  ///< 锁等待超时（秒）；0 表示默认
  time_t busy_timeout = 0;
  ///< 数据库文件路径，:memory: 为内存库
  std::string database_path = ":memory:";
  ///< 透传 SOCI session 选项
  std::map<std::string, std::string> soci_options;
};

/// @brief MySQL 连接配置
struct MySqlConfig {
  ///< 端口
  int port = 3306;
  ///< 用户名
  std::string user = "root";
  ///< 密码
  std::string password = "";
  ///< 主机地址
  std::string host = "127.0.0.1";
  ///< 默认库名
  std::string database_name = "qtrade";
  ///< 连接超时（秒）；0 表示默认
  time_t connect_timeout = 0;
  ///< 透传 SOCI session 选项
  std::map<std::string, std::string> soci_options;
};

/// @brief PostgreSQL 连接配置
struct PostgreSqlConfig {
  ///< 端口
  int port = 5432;
  ///< 用户名
  std::string user = "postgres";
  ///< 密码
  std::string password = "";
  ///< TCP 主机地址
  std::string host = "127.0.0.1";
  ///< 数据库名（dbname）
  std::string database_name = "qtrade";
  ///< Unix socket 路径（kUnix 时使用）
  std::string socket_path = "";
  ///< sslmode
  std::string ssl_mode = "disable";
  ///< 连接超时（秒）；0 表示默认
  time_t connect_timeout = 0;
  ///< 透传 SOCI session 选项
  std::map<std::string, std::string> soci_options;
  ///< 连接方式
  ConnectionType connection_type = ConnectionType::kTcp;
};

/// @brief Oracle 连接配置
struct OracleConfig {
  ///< 端口
  int port = 1521;
  ///< 用户名
  std::string user;
  ///< 密码
  std::string password = "";
  ///< 主机地址
  std::string host = "127.0.0.1";
  ///< 服务名（SOCI service）
  std::string service_name = "ORCL";
  ///< 连接超时（秒）；0 表示默认
  time_t connect_timeout = 0;
  ///< 透传 SOCI session 选项
  std::map<std::string, std::string> soci_options;
};

/// @brief 通用连接配置
struct ConnectionConfig {
  ///< 数据库类型
  DatabaseType database_type = DatabaseType::kSqlite3;
  ///< 连接串
  std::string conn_string;
  ///< 驱动 session 选项
  std::map<std::string, std::string> soci_options;

  ConnectionConfig() = default;

  /// @brief 由 SQLite 配置构造
  /// @param config SQLite 配置
  explicit ConnectionConfig(const SqliteConfig& config);

  /// @brief 由 MySQL 配置构造
  /// @param config MySQL 配置
  explicit ConnectionConfig(const MySqlConfig& config);

  /// @brief 由 PostgreSQL 配置构造
  /// @param config PostgreSQL 配置
  explicit ConnectionConfig(const PostgreSqlConfig& config);

  /// @brief 由 Oracle 配置构造
  /// @param config Oracle 配置
  explicit ConnectionConfig(const OracleConfig& config);
};

/// @brief 连接池配置（单连接配置 + 池行为）
struct ConnectionPoolConfig {
  ///< 连接池大小
  std::size_t pool_size = 4;
  ///< 借连接最长等待（秒）；0 表示池满时立即返回 nullptr
  time_t lease_timeout = 0;
  ///< 单连接配置
  ConnectionConfig connection;

  ConnectionPoolConfig() = default;

  /// @brief 由单连接配置构造
  /// @param connection 单连接配置
  explicit ConnectionPoolConfig(ConnectionConfig connection);

  /// @brief 显式指定连接配置与池大小
  /// @param connection 单连接配置
  /// @param pool_size 连接池大小
  ConnectionPoolConfig(ConnectionConfig connection, std::size_t pool_size);
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_CONFIG_HPP_
