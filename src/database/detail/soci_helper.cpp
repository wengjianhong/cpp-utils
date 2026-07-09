#include "src/database/detail/soci_helper.hpp"

#include "src/database/detail/result_set_impl.hpp"

namespace cpp_utils::database::detail {

Value FromSociField(const soci::row& row, std::size_t index) {
  Value value;
  if (row.get_indicator(index) == soci::i_null) {
    return value;
  }

  const auto props = row.get_properties(index);
  switch (props.get_data_type()) {
    case soci::dt_integer:
      value.type = Value::Type::kInt64;
      value.int64_value = row.get<int>(index);
      break;
    case soci::dt_long_long:
      value.type = Value::Type::kInt64;
      value.int64_value = row.get<long long>(index);
      break;
    case soci::dt_unsigned_long_long:
      value.type = Value::Type::kInt64;
      value.int64_value = static_cast<std::int64_t>(row.get<unsigned long long>(index));
      break;
    case soci::dt_double:
      value.type = Value::Type::kDouble;
      value.double_value = row.get<double>(index);
      break;
    case soci::dt_string:
      value.type = Value::Type::kString;
      value.string_value = row.get<std::string>(index);
      break;
    default:
      value.type = Value::Type::kString;
      value.string_value = row.get<std::string>(index);
      break;
  }
  return value;
}

std::shared_ptr<ColumnSchema> BuildColumnSchema(const soci::row& row) {
  auto schema = std::make_shared<ColumnSchema>();
  schema->names.reserve(row.size());
  for (std::size_t i = 0; i < row.size(); ++i) {
    schema->names.push_back(row.get_properties(i).get_name());
  }
  schema->name_to_index.reserve(schema->names.size());
  for (std::size_t i = 0; i < schema->names.size(); ++i) {
    schema->name_to_index.emplace(schema->names[i], i);
  }
  return schema;
}

bool ExecuteSql(soci::session& session,
                const std::string& sql,
                std::int64_t* affected_rows,
                DbError& last_error,
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
                                     DbError& last_error,
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

}  // namespace cpp_utils::database::detail
