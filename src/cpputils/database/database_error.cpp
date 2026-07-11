/// @file      database_error.cpp
/// @brief     DatabaseError 成员实现
/// @author    wengjianhong
/// @date      2026-07-09
/// @copyright CC BY-NC-SA 4.0
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
