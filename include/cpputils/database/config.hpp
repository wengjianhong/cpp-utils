/// @file      config.hpp
/// @brief     数据库连接配置与 conn_string 构建
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_CONFIG_HPP_
#define CPP_UTILS_DATABASE_CONFIG_HPP_

#include <cpputils/database/types.hpp>

#include <cstddef>
#include <ctime>
#include <map>
#include <string>

namespace cpp_utils::database {

/// @brief PostgreSQL 连接方式
enum class PostgreSqlConnectionType {
  kTcp,   ///< TCP
  kUnix,  ///< Unix socket
};

/// @brief SQLite 连接配置
struct SqliteConfig {
  time_t busy_timeout = 0;                          ///< 锁等待超时（秒）；0 表示默认
  std::string database_path = ":memory:";           ///< 数据库文件路径，:memory: 为内存库
  std::map<std::string, std::string> soci_options;  ///< 透传 SOCI session 选项
};

/// @brief MySQL 连接配置
struct MySqlConfig {
  int port = 3306;                                  ///< 端口
  std::string user = "root";                        ///< 用户名
  std::string password = "";                        ///< 密码
  std::string host = "127.0.0.1";                   ///< 主机地址
  std::string database_name = "qtrade";             ///< 默认库名
  time_t connect_timeout = 0;                       ///< 连接超时（秒）；0 表示默认
  std::map<std::string, std::string> soci_options;  ///< 透传 SOCI session 选项
};

/// @brief PostgreSQL 连接配置
struct PostgreSqlConfig {
  int port = 5432;                                  ///< 端口
  std::string user = "postgres";                    ///< 用户名
  std::string password = "";                        ///< 密码
  std::string host = "127.0.0.1";                   ///< TCP 主机地址
  std::string database_name = "qtrade";             ///< 数据库名（dbname）
  std::string socket_path = "";                     ///< Unix socket 路径（kUnix 时使用）
  std::string ssl_mode = "disable";                 ///< sslmode
  time_t connect_timeout = 0;                       ///< 连接超时（秒）；0 表示默认
  std::map<std::string, std::string> soci_options;  ///< 透传 SOCI session 选项
  PostgreSqlConnectionType connection_type = PostgreSqlConnectionType::kTcp;  ///< 连接方式
};

/// @brief Oracle 连接配置
struct OracleConfig {
  int port = 1521;                                  ///< 端口
  std::string user;                                 ///< 用户名
  std::string password = "";                        ///< 密码
  std::string host = "127.0.0.1";                   ///< 主机地址
  std::string service_name = "ORCL";                ///< 服务名（SOCI service）
  time_t connect_timeout = 0;                       ///< 连接超时（秒）；0 表示默认
  std::map<std::string, std::string> soci_options;  ///< 透传 SOCI session 选项
};

/// @brief 通用连接参数（conn_string 遵循 SOCI 约定）
struct ConnectionOptions {
  DatabaseType database_type = DatabaseType::kSqlite3;  ///< 数据库类型
  std::string conn_string;                              ///< SOCI 连接串
  std::map<std::string, std::string> soci_options;      ///< 透传 SOCI session 选项

  ConnectionOptions() = default;

  /// @brief 由 SQLite 配置构造
  /// @param config SQLite 配置
  explicit ConnectionOptions(const SqliteConfig& config);

  /// @brief 由 MySQL 配置构造
  /// @param config MySQL 配置
  explicit ConnectionOptions(const MySqlConfig& config);

  /// @brief 由 PostgreSQL 配置构造
  /// @param config PostgreSQL 配置
  explicit ConnectionOptions(const PostgreSqlConfig& config);

  /// @brief 由 Oracle 配置构造
  /// @param config Oracle 配置
  explicit ConnectionOptions(const OracleConfig& config);
};

/// @brief 连接池配置（单连接参数 + 池行为）
struct ConnectionPoolOptions {
  std::size_t pool_size = 4;     ///< 连接池大小
  time_t lease_timeout = 0;      ///< 借连接最长等待（秒）；0 表示池满时立即返回 nullptr
  ConnectionOptions connection;  ///< 单连接参数

  ConnectionPoolOptions() = default;

  /// @brief 由单连接参数构造
  /// @param connection 单连接参数
  explicit ConnectionPoolOptions(ConnectionOptions connection);

  /// @brief 显式指定连接参数与池大小
  /// @param connection 单连接参数
  /// @param pool_size 连接池大小
  ConnectionPoolOptions(ConnectionOptions connection, std::size_t pool_size);
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_CONFIG_HPP_
