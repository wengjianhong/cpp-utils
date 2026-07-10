#include "src/database/result_set_impl.hpp"
#include "src/database/result_header_impl.hpp"
#include "src/database/result_row_impl.hpp"

#include "src/database/soci_helper.hpp"

namespace cpputils::database::detail {

SociResultSet::SociResultSet(soci::rowset<soci::row>&& rowset)
  : rowset_(std::move(rowset)), it_(rowset_.begin()), end_(rowset_.end()) {}

std::shared_ptr<const IResultHeader> SociResultSet::Header() const {
  return header_;
}

std::size_t SociResultSet::column_count() const {
  return header_ ? header_->size() : 0;
}

std::optional<std::unique_ptr<IResultRow>> SociResultSet::Fetch() {
  if (it_ == end_) {
    return std::nullopt;
  }

  const soci::row& srow = *it_;
  if (!header_) {
    header_ = BuildResultHeader(srow);
  }

  std::vector<ResultValue> values;
  values.reserve(srow.size());
  for (std::size_t i = 0; i < srow.size(); ++i) {
    values.push_back(FromSociField(srow, i));
  }

  ++it_;
  return std::make_unique<ResultRowImpl>(header_, std::move(values));
}

}  // namespace cpputils::database::detail
