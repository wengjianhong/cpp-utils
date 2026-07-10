/// @file      database_types.cpp
/// @brief     DatabaseType 辅助函数实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include <cpputils/database/database_types.hpp>

namespace cpputils::database {

std::string_view GetDatabaseTypeName(DatabaseType type) noexcept {
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

}  // namespace cpputils::database
