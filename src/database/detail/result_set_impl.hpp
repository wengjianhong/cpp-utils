#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_

#include "include/database/result_set.hpp"

#include <soci/row.h>
#include <soci/rowset.h>

namespace cpp_utils::database::detail {

class SociResultSet final : public IResultSet {
 public:
  explicit SociResultSet(soci::rowset<soci::row>&& rowset);

  [[nodiscard]] std::shared_ptr<const ColumnSchema> Schema() const override;
  [[nodiscard]] std::size_t column_count() const override;
  [[nodiscard]] std::optional<Row> Fetch() override;

 private:
  std::shared_ptr<const ColumnSchema> schema_;
  soci::rowset<soci::row> rowset_;
  soci::rowset<soci::row>::const_iterator it_;
  soci::rowset<soci::row>::const_iterator end_;
};

}  // namespace cpp_utils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_SET_IMPL_HPP_
