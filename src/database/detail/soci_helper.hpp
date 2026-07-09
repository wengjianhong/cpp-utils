#ifndef CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
#define CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_

#include <cpputils/database/column_schema.hpp>
#include <cpputils/database/error.hpp>
#include <cpputils/database/result_set.hpp>
#include <cpputils/database/types.hpp>
#include <cpputils/database/value.hpp>

#include <soci/soci.h>

#include <memory>
#include <string>
#include <utility>

namespace soci {
class session;
}

namespace cpp_utils::database::detail {

inline void SetDbErrorMessage(DbError& err, DatabaseType database_type, std::string message) {
  err.native_code = 0;
  err.database_type = database_type;
  err.message = std::move(message);
  err.sqlstate.clear();
}

inline void SetDbErrorFromSoci(DbError& err, DatabaseType database_type, const soci::soci_error& ex) {
  SetDbErrorMessage(err, database_type, ex.what());
}

[[nodiscard]] Value FromSociField(const soci::row& row, std::size_t index);

[[nodiscard]] std::shared_ptr<ColumnSchema> BuildColumnSchema(const soci::row& row);

[[nodiscard]] bool ExecuteSql(soci::session& session,
                               const std::string& sql,
                               std::int64_t* affected_rows,
                               DbError& last_error,
                               DatabaseType database_type);

[[nodiscard]] std::unique_ptr<IResultSet> QuerySql(soci::session& session,
                                                   const std::string& sql,
                                                   DbError& last_error,
                                                   DatabaseType database_type);

}  // namespace cpp_utils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
