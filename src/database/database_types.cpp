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
