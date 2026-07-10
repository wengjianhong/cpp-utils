/// @file      result_row.hpp
/// @brief     查询结果单行数据接口
/// @details   按列下标或列名读取单元格
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_RESULT_ROW_HPP_
#define CPP_UTILS_DATABASE_RESULT_ROW_HPP_

#include <cpputils/database/result_value.hpp>

#include <cstddef>
#include <optional>
#include <string_view>

namespace cpputils::database {

/// @brief 单行数据
class IResultRow {
 public:
  virtual ~IResultRow() = default;

  /// @brief 按下标读取单元格
  [[nodiscard]] virtual std::optional<ResultValue> get_value(std::size_t index) const = 0;

  /// @brief 按列名读取单元格
  [[nodiscard]] virtual std::optional<ResultValue> get_value(std::string_view name) const = 0;
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_RESULT_ROW_HPP_
