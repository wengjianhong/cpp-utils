/// @file      result_header_impl.cpp
/// @brief     IResultHeader SOCI 实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include "cpputils/database/result_header_impl.hpp"

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
