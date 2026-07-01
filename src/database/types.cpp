#include <cpputils/database/types.hpp>

namespace cpp_utils::database {

std::string_view ToSociBackendName(DatabaseType type) noexcept {
  switch (type) {
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

}  // namespace cpp_utils::database
