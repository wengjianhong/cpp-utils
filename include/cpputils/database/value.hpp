/// @file      value.hpp
/// @brief     数据库单元格值的类型擦除表示
/// @details   公共 API 不暴露 SOCI 类型；类型转换统一使用 as_xxx
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_VALUE_HPP_
#define CPP_UTILS_DATABASE_VALUE_HPP_

#include <cstdint>
#include <optional>
#include <string>

namespace cpp_utils::database {

/// @brief 单元格值（tagged union）
struct Value {
  /// @brief 值类型标签
  enum class Type {
    ///< SQL NULL
    kNull = 0,
    ///< 64 位整数型
    kInt64 = 1,
    ///< 浮点型
    kDouble = 2,
    ///< 字符串型
    kString = 3,
  };

  ///< 当前类型标签
  Type type = Type::kNull;
  ///< kInt64 载荷
  std::int64_t int64_value = 0;
  ///< kDouble 载荷
  double double_value = 0.0;
  ///< kString 载荷
  std::string string_value;

  /// @brief 是否为 NULL
  /// @return true 表示 kNull
  [[nodiscard]] bool is_null() const;

  /// @brief 转为 int64（仅 kInt64）
  /// @return 值；类型不匹配或 NULL 时 nullopt
  [[nodiscard]] std::optional<std::int64_t> as_int64() const;

  /// @brief 转为 double（kDouble；kInt64 可提升）
  /// @return 值；无法转换或 NULL 时 nullopt
  [[nodiscard]] std::optional<double> as_double() const;

  /// @brief 转为字符串（含数值格式化）
  /// @return 字符串；NULL 时 nullopt
  [[nodiscard]] std::optional<std::string> as_string() const;
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_VALUE_HPP_
