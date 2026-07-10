/// @file      result_set.hpp
/// @brief     流式查询结果集接口
/// @details   逐行 Fetch，内存占用 O(1) 行
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_RESULT_SET_HPP_
#define CPP_UTILS_DATABASE_RESULT_SET_HPP_

#include <cpputils/database/result_header.hpp>
#include <cpputils/database/result_row.hpp>

#include <cstddef>
#include <memory>
#include <optional>

namespace cpputils::database {

/// @brief 流式结果集：调用方逐行 Fetch
class IResultSet {
 public:
  virtual ~IResultSet() = default;

  /// @brief 获取列 header（全结果集共享）
  /// @return 列元数据 shared_ptr
  [[nodiscard]] virtual std::shared_ptr<const IResultHeader> Header() const = 0;

  /// @brief 列数量
  /// @return 列数量
  [[nodiscard]] virtual std::size_t column_count() const = 0;

  /// @brief 读取下一行
  /// @return 下一行；nullopt 表示结果集结束
  [[nodiscard]] virtual std::optional<std::unique_ptr<IResultRow>> Fetch() = 0;
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_RESULT_SET_HPP_
