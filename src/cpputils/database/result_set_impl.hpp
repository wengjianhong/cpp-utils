/// @file      result_set_impl.hpp
/// @brief     IResultSet 的 SOCI 流式实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_

#include <cpputils/database/result_set.hpp>

#include <soci/row.h>
#include <soci/rowset.h>

namespace cpputils::database::detail {

/// @brief IResultSet 的 SOCI rowset 实现（逐行 Fetch，O(1) 行内存）
class SociResultSet final : public IResultSet {
 public:
  /// @brief 构造流式结果集
  /// @param rowset SOCI rowset（移动语义）
  explicit SociResultSet(soci::rowset<soci::row>&& rowset);

  /// @brief 获取列 header
  /// @return 列元数据；首行 Fetch 前可能为空 shared_ptr
  [[nodiscard]] std::shared_ptr<const IResultHeader> Header() const override;

  /// @brief 列数量
  /// @return 列个数；header 未构建时返回 0
  [[nodiscard]] std::size_t column_count() const override;

  /// @brief 读取下一行
  /// @return 下一行；nullopt 表示结果集结束
  [[nodiscard]] std::optional<std::unique_ptr<IResultRow>> Fetch() override;

 private:
  std::shared_ptr<const IResultHeader> header_;  ///< 懒构建的列 header
  soci::rowset<soci::row> rowset_;               ///< SOCI 结果集
  soci::rowset<soci::row>::const_iterator it_;   ///< 当前行迭代器
  soci::rowset<soci::row>::const_iterator end_;  ///< 结束迭代器
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_
