/// @file      result_row_impl.hpp
/// @brief     IResultRow 的 SOCI 实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_

#include <cpputils/database/result_row.hpp>

#include <memory>
#include <vector>

namespace cpputils::database {

class IResultHeader;

}  // namespace cpputils::database

namespace cpputils::database::detail {

/// @brief IResultRow 的默认实现（持有 header 与列值向量）
class ResultRowImpl final : public IResultRow {
 public:
  /// @brief 构造一行
  /// @param header 所属结果集 header（按列名解析）
  /// @param values 列值列表
  ResultRowImpl(std::shared_ptr<const IResultHeader> header, std::vector<ResultValue> values);

  /// @brief 按下标读取单元格
  /// @param index 列下标
  /// @return 单元格值；越界时 nullopt
  [[nodiscard]] std::optional<ResultValue> get_value(std::size_t index) const override;

  /// @brief 按列名读取单元格
  /// @param name 列名
  /// @return 单元格值；列名不存在时 nullopt
  [[nodiscard]] std::optional<ResultValue> get_value(std::string_view name) const override;

 private:
  /// @brief 按列名解析索引
  /// @param name 列名
  /// @return 列下标；header 为空或列名不存在时 nullopt
  [[nodiscard]] std::optional<std::size_t> ResolveIndex(std::string_view name) const;

  std::shared_ptr<const IResultHeader> header_;  ///< 列元数据
  std::vector<ResultValue> values_;            ///< 列值
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_
