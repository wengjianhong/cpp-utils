/// @file      database_types.cpp
/// @brief     DatabaseType 辅助函数实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include <cpputils/database/database_types.hpp>

namespace cpputils::database {

std::string_view GetDatabaseNameByType(DatabaseType type) noexcept {
  switch (type) {
    case DatabaseType::kUnknown:
      return "";
    case DatabaseType::kMySql:
      return "mysql";
    case DatabaseType::kSqlite3:
      return "sqlite3";
    case DatabaseType::kPostgreSql:
      return "postgresql";
    case DatabaseType::kOracle:
      return "oracle";
    case DatabaseType::kOdbc:
      return "odbc";
  }
  return "";
}

DatabaseType GetDatabaseTypeByName(std::string_view name) noexcept {
  if (name == "mysql") {
    return DatabaseType::kMySql;
  }
  if (name == "sqlite3" || name == "sqlite") {
    return DatabaseType::kSqlite3;
  }
  if (name == "postgresql" || name == "postgres") {
    return DatabaseType::kPostgreSql;
  }
  if (name == "oracle") {
    return DatabaseType::kOracle;
  }
  if (name == "odbc") {
    return DatabaseType::kOdbc;
  }
  return DatabaseType::kUnknown;
}

}  // namespace cpputils::database
