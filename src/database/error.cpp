#include <cpputils/database/error.hpp>

namespace cpp_utils::database {

bool DbError::Ok() const noexcept {
  return native_code == 0 && message.empty() && sqlstate.empty();
}

void DbError::Clear() noexcept {
  native_code = 0;
  database_type = DatabaseType::kUnknown;
  message.clear();
  sqlstate.clear();
}

}  // namespace cpp_utils::database
