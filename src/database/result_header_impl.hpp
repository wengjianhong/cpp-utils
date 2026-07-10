#ifndef CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_

#include <cpputils/database/result_header.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace cpputils::database::detail {

class ResultHeaderImpl final : public IResultHeader {
 public:
  [[nodiscard]] std::optional<std::size_t> IndexOf(std::string_view name) const override;
  [[nodiscard]] std::size_t size() const override;

  std::vector<std::string> names;
  std::unordered_map<std::string_view, std::size_t> name_to_index;
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_RESULT_HEADER_IMPL_HPP_
