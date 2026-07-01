#include "include/database/config.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace {

std::string BuildConnString(const cpp_utils::database::SqliteConfig& config) {
  if (config.database_path == ":memory:") {
    return ":memory:";
  }
  return "dbname=" + config.database_path;
}

std::string BuildConnString(const cpp_utils::database::MySqlConfig& config) {
  std::ostringstream ss;
  ss << "host=" << config.host << " port=" << config.port << " db=" << config.database_name
     << " user=" << config.user << " password=" << config.password;
  if (config.connect_timeout > 0) {
    ss << " connect_timeout=" << config.connect_timeout;
  }
  return ss.str();
}

std::string BuildConnString(const cpp_utils::database::PostgreSqlConfig& config) {
  std::ostringstream ss;
  if (config.connection_type == cpp_utils::database::PostgreSqlConnectionType::kUnix) {
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

std::string BuildConnString(const cpp_utils::database::OracleConfig& config) {
  std::ostringstream ss;
  ss << "host=" << config.host << " port=" << config.port << " service=" << config.service_name
     << " user=" << config.user << " password=" << config.password;
  if (config.connect_timeout > 0) {
    ss << " connect_timeout=" << config.connect_timeout;
  }
  return ss.str();
}

}  // namespace

namespace cpp_utils::database {

ConnectionOptions::ConnectionOptions(const SqliteConfig& config)
    : database_type(DatabaseType::kSqlite3),
      conn_string(BuildConnString(config)),
      soci_options(config.soci_options) {
  if (config.busy_timeout > 0) {
    soci_options["sqlite3.busy_timeout"] = std::to_string(config.busy_timeout * 1000);
  }
}

ConnectionOptions::ConnectionOptions(const MySqlConfig& config)
    : database_type(DatabaseType::kMySql),
      conn_string(BuildConnString(config)),
      soci_options(config.soci_options) {}

ConnectionOptions::ConnectionOptions(const PostgreSqlConfig& config)
    : database_type(DatabaseType::kPostgreSql),
      conn_string(BuildConnString(config)),
      soci_options(config.soci_options) {}

ConnectionOptions::ConnectionOptions(const OracleConfig& config)
    : database_type(DatabaseType::kOracle),
      conn_string(BuildConnString(config)),
      soci_options(config.soci_options) {}

ConnectionPoolOptions::ConnectionPoolOptions(ConnectionOptions connection) : connection(std::move(connection)) {}

ConnectionPoolOptions::ConnectionPoolOptions(ConnectionOptions connection, std::size_t pool_size)
    : pool_size(pool_size), connection(std::move(connection)) {}

}  // namespace cpp_utils::database
