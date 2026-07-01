#include "include/database/value.hpp"

namespace cpp_utils::database {

bool Value::is_null() const {
  return type == Type::kNull;
}

std::optional<std::int64_t> Value::as_int64() const {
  if (type == Type::kInt64) {
    return int64_value;
  }
  return std::nullopt;
}

std::optional<double> Value::as_double() const {
  if (is_null()) {
    return std::nullopt;
  }
  if (type == Type::kDouble) {
    return double_value;
  }
  if (type == Type::kInt64) {
    return static_cast<double>(int64_value);
  }
  return std::nullopt;
}

std::optional<bool> Value::as_bool() const {
  if (is_null()) {
    return std::nullopt;
  }
  if (type == Type::kBool) {
    return bool_value;
  }
  if (type == Type::kInt64) {
    return int64_value != 0;
  }
  return std::nullopt;
}

std::optional<std::string> Value::as_string() const {
  if (type == Type::kString) {
    return string_value;
  }
  if (type == Type::kInt64) {
    return std::to_string(int64_value);
  }
  if (type == Type::kDouble) {
    return std::to_string(double_value);
  }
  if (type == Type::kBool) {
    return bool_value ? "true" : "false";
  }
  return std::nullopt;
}

}  // namespace cpp_utils::database
