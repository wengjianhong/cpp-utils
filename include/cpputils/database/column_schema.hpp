/// @file      column_schema.hpp
/// @brief     结果集列元数据
/// @details   全结果集共享一份 schema，避免每行重复列名
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_COLUMN_SCHEMA_HPP_
#define CPP_UTILS_DATABASE_COLUMN_SCHEMA_HPP_

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cpp_utils::database {

/// @brief 结果集列元数据（列名与下标映射）
struct ColumnSchema {
  ///< 列名列表，下标即列索引
  std::vector<std::string> names;
  ///< 列名 -> 索引
  std::unordered_map<std::string_view, std::size_t> name_to_index;

  /// @brief 按列名查找索引
  /// @param name 列名
  /// @return 列下标；不存在时 nullopt
  [[nodiscard]] std::optional<std::size_t> IndexOf(std::string_view name) const;

  /// @brief 列数量
  /// @return 列个数
  [[nodiscard]] std::size_t size() const;
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_COLUMN_SCHEMA_HPP_
