/// @file      config.cpp
/// @brief     ConnectionConfig / ConnectionPoolConfig 构造与 conn_string 构建
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include <cpputils/database/config.hpp>

#include <sstream>
#include <string>
#include <utility>

namespace {

/// @brief 由 SQLite 配置构建 SOCI 连接串
/// @param config SQLite 配置
/// @return SOCI conn_string
std::string BuildConnString(const cpputils::database::SqliteConfig& config) {
  if (config.database_path == ":memory:") {
    return ":memory:";
  }
  return "dbname=" + config.database_path;
}

/// @brief 由 MySQL 配置构建 SOCI 连接串
/// @param config MySQL 配置
/// @return SOCI conn_string
std::string BuildConnString(const cpputils::database::MySqlConfig& config) {
  std::ostringstream ss;
  ss << "host=" << config.host << " port=" << config.port << " db=" << config.database_name << " user=" << config.user
     << " password=" << config.password;
  if (config.connect_timeout > 0) {
    ss << " connect_timeout=" << config.connect_timeout;
  }
  return ss.str();
}

/// @brief 由 PostgreSQL 配置构建 SOCI 连接串
/// @param config PostgreSQL 配置
/// @return SOCI conn_string
std::string BuildConnString(const cpputils::database::PostgreSqlConfig& config) {
  std::ostringstream ss;
  if (config.connection_type == cpputils::database::ConnectionType::kUnix) {
    ss << "host=" << config.socket_path << " port=" << config.port << " ";
  } else {
    ss << "host=" << config.host << " port=" << config.port << " ";
  }
  if (!config.database_name.empty()) {
    ss << "dbname=" << config.database_name << " ";
  }
  if (!config.user.empty()) {
    ss << "user=" << config.user << " ";
  }
  if (!config.password.empty()) {
    ss << "password=" << config.password << " ";
  }
  if (!config.ssl_mode.empty()) {
    ss << "sslmode=" << config.ssl_mode << " ";
  }
  if (config.connect_timeout > 0) {
    ss << "connect_timeout=" << config.connect_timeout << " ";
  }
  return ss.str();
}

/// @brief 由 Oracle 配置构建 SOCI 连接串
/// @param config Oracle 配置
/// @return SOCI conn_string
std::string BuildConnString(const cpputils::database::OracleConfig& config) {
  std::ostringstream ss;
  ss << "host=" << config.host << " port=" << config.port << " service=" << config.service_name
     << " user=" << config.user << " password=" << config.password;
  if (config.connect_timeout > 0) {
    ss << " connect_timeout=" << config.connect_timeout;
  }
  return ss.str();
}

}  // namespace

namespace cpputils::database {

ConnectionConfig::ConnectionConfig(const SqliteConfig& config)
  : database_type(DatabaseType::kSqlite3), conn_string(BuildConnString(config)), soci_options(config.soci_options) {
  if (config.busy_timeout > 0) {
    soci_options["sqlite3.busy_timeout"] = std::to_string(config.busy_timeout * 1000);
  }
}

ConnectionConfig::ConnectionConfig(const MySqlConfig& config)
  : database_type(DatabaseType::kMySql), conn_string(BuildConnString(config)), soci_options(config.soci_options) {}

ConnectionConfig::ConnectionConfig(const PostgreSqlConfig& config)
  : database_type(DatabaseType::kPostgreSql), conn_string(BuildConnString(config)), soci_options(config.soci_options) {}

ConnectionConfig::ConnectionConfig(const OracleConfig& config)
  : database_type(DatabaseType::kOracle), conn_string(BuildConnString(config)), soci_options(config.soci_options) {}

ConnectionPoolConfig::ConnectionPoolConfig(ConnectionConfig connection) : connection(std::move(connection)) {}

ConnectionPoolConfig::ConnectionPoolConfig(ConnectionConfig connection, std::size_t pool_size)
  : pool_size(pool_size), connection(std::move(connection)) {}

}  // namespace cpputils::database
