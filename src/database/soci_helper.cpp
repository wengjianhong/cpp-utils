/// @file      soci_helper.cpp
/// @brief     SOCI 转换与 Execute/Query 辅助实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include "src/database/soci_helper.hpp"

#include "src/database/result_header_impl.hpp"
#include "src/database/result_set_impl.hpp"

namespace cpputils::database::detail {

ResultValue FromSociField(const soci::row& row, std::size_t index) {
  ResultValue value;
  if (row.get_indicator(index) == soci::i_null) {
    return value;
  }

  const auto props = row.get_properties(index);
  switch (props.get_data_type()) {
    case soci::dt_integer:
      value.type = ResultValue::Type::kInt64;
      value.int64_value = row.get<int>(index);
      break;
    case soci::dt_long_long:
      value.type = ResultValue::Type::kInt64;
      value.int64_value = row.get<long long>(index);
      break;
    case soci::dt_unsigned_long_long:
      value.type = ResultValue::Type::kInt64;
      value.int64_value = static_cast<std::int64_t>(row.get<unsigned long long>(index));
      break;
    case soci::dt_double:
      value.type = ResultValue::Type::kDouble;
      value.double_value = row.get<double>(index);
      break;
    case soci::dt_string:
      value.type = ResultValue::Type::kString;
      value.string_value = row.get<std::string>(index);
      break;
    default:
      value.type = ResultValue::Type::kString;
      value.string_value = row.get<std::string>(index);
      break;
  }
  return value;
}

std::shared_ptr<IResultHeader> BuildResultHeader(const soci::row& row) {
  auto header = std::make_shared<ResultHeaderImpl>();
  header->names.reserve(row.size());
  for (std::size_t i = 0; i < row.size(); ++i) {
    header->names.push_back(row.get_properties(i).get_name());
  }
  header->name_to_index.reserve(header->names.size());
  for (std::size_t i = 0; i < header->names.size(); ++i) {
    header->name_to_index.emplace(header->names[i], i);
  }
  return header;
}

bool ExecuteSql(soci::session& session,
                const std::string& sql,
                std::int64_t* affected_rows,
                DatabaseError& last_error,
                DatabaseType database_type) {
  try {
    soci::statement st = (session.prepare << sql);
    st.execute(true);
    if (affected_rows != nullptr) {
      *affected_rows = st.get_affected_rows();
    }
    last_error.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    SetDbErrorFromSoci(last_error, database_type, ex);
    return false;
  }
}

std::unique_ptr<IResultSet> QuerySql(soci::session& session,
                                     const std::string& sql,
                                     DatabaseError& last_error,
                                     DatabaseType database_type) {
  try {
    soci::rowset<soci::row> rs = (session.prepare << sql);
    last_error.Clear();
    return std::make_unique<SociResultSet>(std::move(rs));
  } catch (const soci::soci_error& ex) {
    SetDbErrorFromSoci(last_error, database_type, ex);
    return nullptr;
  }
}

}  // namespace cpputils::database::detail
