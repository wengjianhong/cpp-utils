/// @file      soci_helper.hpp
/// @brief     SOCI 与 cpputils 数据库类型的转换辅助
/// @details   仅库内使用；ExecuteSql/QuerySql 统一捕获 soci_error 并写入 DatabaseError
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
#define CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_

#include <cpputils/database/database_error.hpp>
#include <cpputils/database/database_types.hpp>
#include <cpputils/database/result_header.hpp>
#include <cpputils/database/result_set.hpp>
#include <cpputils/database/result_value.hpp>

#include <soci/soci.h>

#include <memory>
#include <string>
#include <utility>

namespace soci {
class session;
}

namespace cpputils::database::detail {

/// @brief 写入自定义错误消息
/// @param err 目标错误对象
/// @param database_type 数据库类型
/// @param message 错误描述
inline void SetDbErrorMessage(DatabaseError& err, DatabaseType database_type, std::string message) {
  err.native_code = 0;
  err.database_type = database_type;
  err.message = std::move(message);
  err.sqlstate.clear();
}

/// @brief 从 SOCI 异常写入错误详情
/// @param err 目标错误对象
/// @param database_type 数据库类型
/// @param ex SOCI 异常
inline void SetDbErrorFromSoci(DatabaseError& err, DatabaseType database_type, const soci::soci_error& ex) {
  SetDbErrorMessage(err, database_type, ex.what());
}

/// @brief 将 SOCI 字段转换为 ResultValue
/// @param row SOCI 结果行
/// @param index 列下标
/// @return 转换后的值；NULL 列返回默认构造 ResultValue
[[nodiscard]] ResultValue FromSociField(const soci::row& row, std::size_t index);

/// @brief 从 SOCI 首行构建结果集 header
/// @param row SOCI 结果行（用于读取列名）
/// @return 列元数据 shared_ptr
[[nodiscard]] std::shared_ptr<IResultHeader> BuildResultHeader(const soci::row& row);

/// @brief 执行非查询 SQL
/// @param session SOCI 会话
/// @param sql SQL 语句
/// @param affected_rows 可选，写入受影响行数
/// @param last_error 失败时写入错误详情
/// @param database_type 数据库类型
/// @return true 表示成功
[[nodiscard]] bool ExecuteSql(soci::session& session,
                              const std::string& sql,
                              std::int64_t* affected_rows,
                              DatabaseError& last_error,
                              DatabaseType database_type);

/// @brief 执行查询 SQL 并返回流式结果集
/// @param session SOCI 会话
/// @param sql SQL 语句
/// @param last_error 失败时写入错误详情
/// @param database_type 数据库类型
/// @return 成功时非空结果集；失败时 nullptr
[[nodiscard]] std::unique_ptr<IResultSet> QuerySql(soci::session& session,
                                                   const std::string& sql,
                                                   DatabaseError& last_error,
                                                   DatabaseType database_type);

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_SOCI_HELPER_HPP_
