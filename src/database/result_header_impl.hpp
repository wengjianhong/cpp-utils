/// @file      result_header_impl.hpp
/// @brief     IResultHeader 的 SOCI 实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_

#include <cpputils/database/result_header.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace cpputils::database::detail {

/// @brief IResultHeader 的默认实现（列名列表 + 名称索引）
class ResultHeaderImpl final : public IResultHeader {
 public:
  /// @brief 按列名查找索引
  /// @param name 列名
  /// @return 列下标；不存在时 nullopt
  [[nodiscard]] std::optional<std::size_t> IndexOf(std::string_view name) const override;

  /// @brief 列数量
  /// @return 列个数
  [[nodiscard]] std::size_t size() const override;

  std::vector<std::string> names;                                   ///< 列名列表，下标即列索引
  std::unordered_map<std::string_view, std::size_t> name_to_index;  ///< 列名 -> 索引
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_
