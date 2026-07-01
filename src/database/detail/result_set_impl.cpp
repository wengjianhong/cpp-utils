#include "src/database/detail/result_set_impl.hpp"

#include "src/database/detail/soci_helper.hpp"

namespace cpp_utils::database::detail {

SociResultSet::SociResultSet(soci::rowset<soci::row>&& rowset)
  : rowset_(std::move(rowset)), it_(rowset_.begin()), end_(rowset_.end()) {}

std::shared_ptr<const ColumnSchema> SociResultSet::Schema() const { return schema_; }

std::size_t SociResultSet::column_count() const {
  return schema_ ? schema_->size() : 0;
}

std::optional<Row> SociResultSet::Fetch() {
  if (it_ == end_) {
    return std::nullopt;
  }

  const soci::row& srow = *it_;
  if (!schema_) {
    schema_ = BuildColumnSchema(srow);
  }

  std::vector<Value> values;
  values.reserve(srow.size());
  for (std::size_t i = 0; i < srow.size(); ++i) {
    values.push_back(FromSociField(srow, i));
  }

  ++it_;
  return Row(this, std::move(values));
}

}  // namespace cpp_utils::database::detail
