#include "include/database/column_schema.hpp"

namespace cpp_utils::database {

std::optional<std::size_t> ColumnSchema::IndexOf(std::string_view name) const {
  const auto it = name_to_index.find(name);
  if (it == name_to_index.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::size_t ColumnSchema::size() const {
  return names.size();
}

}  // namespace cpp_utils::database
