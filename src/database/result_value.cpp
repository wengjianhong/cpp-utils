#include <cpputils/database/result_value.hpp>

namespace cpputils::database {

bool ResultValue::is_null() const {
  return type == Type::kNull;
}

std::optional<std::int64_t> ResultValue::as_int64() const {
  if (type == Type::kInt64) {
    return int64_value;
  }
  return std::nullopt;
}

std::optional<double> ResultValue::as_double() const {
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

std::optional<std::string> ResultValue::as_string() const {
  if (type == Type::kString) {
    return string_value;
  }
  if (type == Type::kInt64) {
    return std::to_string(int64_value);
  }
  if (type == Type::kDouble) {
    return std::to_string(double_value);
  }
  return std::nullopt;
}

}  // namespace cpputils::database
