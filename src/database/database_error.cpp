#include <cpputils/database/database_error.hpp>

namespace cpputils::database {

bool DatabaseError::Ok() const noexcept {
  return native_code == 0 && message.empty() && sqlstate.empty();
}

void DatabaseError::Clear() noexcept {
  native_code = 0;
  database_type = DatabaseType::kUnknown;
  message.clear();
  sqlstate.clear();
}

}  // namespace cpputils::database
