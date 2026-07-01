#ifndef CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
#define CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_

#include <cpputils/database/column_schema.hpp>
#include <cpputils/database/error.hpp>
#include <cpputils/database/result_set.hpp>
#include <cpputils/database/value.hpp>

#include <soci/soci.h>

#include <memory>
#include <string>

namespace soci {
class session;
}

namespace cpp_utils::database::detail {

struct TransactionImpl {
  explicit TransactionImpl(soci::session& session) : transaction(session) {}

  soci::transaction transaction;
  bool finished = false;
};

[[nodiscard]] Value FromSociField(const soci::row& row, std::size_t index);

[[nodiscard]] std::shared_ptr<ColumnSchema> BuildColumnSchema(const soci::row& row);

[[nodiscard]] Error ExecuteSql(soci::session& session,
                               const std::string& sql,
                               ExecuteResult* out,
                               std::string& last_error);

[[nodiscard]] std::pair<Error, std::unique_ptr<IResultSet>> QuerySql(soci::session& session,
                                                                     const std::string& sql,
                                                                     std::string& last_error);

}  // namespace cpp_utils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
