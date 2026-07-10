/// @file      result_row_impl.cpp
/// @brief     IResultRow SOCI 实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include "src/database/result_row_impl.hpp"

#include <cpputils/database/result_header.hpp>

#include <utility>

namespace cpputils::database::detail {

ResultRowImpl::ResultRowImpl(std::shared_ptr<const IResultHeader> header, std::vector<ResultValue> values)
  : header_(std::move(header)), values_(std::move(values)) {}

std::optional<std::size_t> ResultRowImpl::ResolveIndex(std::string_view name) const {
  if (!header_) {
    return std::nullopt;
  }
  return header_->IndexOf(name);
}

std::optional<ResultValue> ResultRowImpl::get_value(std::size_t index) const {
  if (index >= values_.size()) {
    return std::nullopt;
  }
  return values_[index];
}

std::optional<ResultValue> ResultRowImpl::get_value(std::string_view name) const {
  const auto index = ResolveIndex(name);
  if (!index.has_value()) {
    return std::nullopt;
  }
  return get_value(index.value());
}

}  // namespace cpputils::database::detail
