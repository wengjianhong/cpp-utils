#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_

#include <cpputils/database/result_row.hpp>

#include <memory>
#include <vector>

namespace cpputils::database {

class IResultHeader;

}  // namespace cpputils::database

namespace cpputils::database::detail {

class ResultRowImpl final : public IResultRow {
 public:
  ResultRowImpl(std::shared_ptr<const IResultHeader> header, std::vector<ResultValue> values);

  [[nodiscard]] std::optional<ResultValue> get_value(std::size_t index) const override;
  [[nodiscard]] std::optional<ResultValue> get_value(std::string_view name) const override;

 private:
  [[nodiscard]] std::optional<std::size_t> ResolveIndex(std::string_view name) const;

  std::shared_ptr<const IResultHeader> header_;
  std::vector<ResultValue> values_;
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_ROW_IMPL_HPP_
