/// @file      row.hpp
/// @brief     查询结果单行数据
/// @details   仅存储列值；列名解析通过关联的 IResultSet::Schema()
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_ROW_HPP_
#define CPP_UTILS_DATABASE_ROW_HPP_

#include "include/database/value.hpp"

#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace cpp_utils::database {

class IResultSet;

/// @brief 单行数据（values_ 与 Fetch 来源的 IResultSet 关联）
class Row {
 public:
  Row() = default;

  /// @brief 构造一行（由 IResultSet::Fetch 使用）
  /// @param result_set 所属结果集，供按列名解析；非拥有指针
  /// @param values 列值
  Row(const IResultSet* result_set, std::vector<Value> values);

  /// @brief 按下标读取单元格
  [[nodiscard]] std::optional<Value> get_value(std::size_t index) const;

  /// @brief 按列名读取单元格（依赖构造时的 IResultSet::Schema()）
  [[nodiscard]] std::optional<Value> get_value(std::string_view name) const;

 private:
  [[nodiscard]] std::optional<std::size_t> ResolveIndex(std::string_view name) const;

  const IResultSet* result_set_ = nullptr;  ///< 非拥有；仅用于 Schema 查询
  std::vector<Value> values_;
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_ROW_HPP_
