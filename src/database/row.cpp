#include <cpputils/database/row.hpp>

#include <cpputils/database/result_set.hpp>

#include <utility>

namespace cpp_utils::database {

Row::Row(const IResultSet* result_set, std::vector<Value> values)
  : result_set_(result_set), values_(std::move(values)) {}

std::optional<std::size_t> Row::ResolveIndex(std::string_view name) const {
  if (result_set_ == nullptr) {
    return std::nullopt;
  }
  const auto schema = result_set_->Schema();
  if (!schema) {
    return std::nullopt;
  }
  return schema->IndexOf(name);
}

std::optional<Value> Row::get_value(std::size_t index) const {
  if (index >= values_.size()) {
    return std::nullopt;
  }
  return values_[index];
}

std::optional<Value> Row::get_value(std::string_view name) const {
  const auto index = ResolveIndex(name);
  if (!index.has_value()) {
    return std::nullopt;
  }
  return get_value(index.value());
}

}  // namespace cpp_utils::database
