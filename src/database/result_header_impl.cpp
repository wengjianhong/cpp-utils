#include "src/database/result_header_impl.hpp"

namespace cpputils::database::detail {

std::optional<std::size_t> ResultHeaderImpl::IndexOf(std::string_view name) const {
  const auto it = name_to_index.find(name);
  if (it == name_to_index.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::size_t ResultHeaderImpl::size() const {
  return names.size();
}

}  // namespace cpputils::database::detail
