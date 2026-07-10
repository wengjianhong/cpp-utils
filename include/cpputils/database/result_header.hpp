/// @file      result_header.hpp
/// @brief     结果集列元数据接口
/// @details   全结果集共享一份 header，避免每行重复列名
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_RESULT_HEADER_HPP_
#define CPP_UTILS_DATABASE_RESULT_HEADER_HPP_

#include <cstddef>
#include <optional>
#include <string_view>

namespace cpputils::database {

/// @brief 结果集列元数据（列名与下标映射）
class IResultHeader {
 public:
  virtual ~IResultHeader() = default;

  /// @brief 按列名查找索引
  /// @param name 列名
  /// @return 列下标；不存在时 nullopt
  [[nodiscard]] virtual std::optional<std::size_t> IndexOf(std::string_view name) const = 0;

  /// @brief 列数量
  /// @return 列个数
  [[nodiscard]] virtual std::size_t size() const = 0;
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_RESULT_HEADER_HPP_
