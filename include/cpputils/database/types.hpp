/// @file      types.hpp
/// @brief     数据库模块公共类型（DatabaseType 等）
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_TYPES_HPP_
#define CPP_UTILS_DATABASE_TYPES_HPP_

#include <string_view>

namespace cpp_utils::database {

/// @brief 支持的数据库类型（对应 SOCI backend 名称）
enum class DatabaseType {
  kMySql,       ///< MySQL
  kSqlite3,     ///< SQLite3
  kPostgreSql,  ///< PostgreSQL
  kOracle,      ///< Oracle
  kOdbc,        ///< ODBC 通用驱动
};

/// @brief 将 DatabaseType 转换为 SOCI backend 字符串
/// @param type 数据库类型
/// @return SOCI 约定的 backend 名称；未知值回退 "sqlite3"
[[nodiscard]] std::string_view ToSociBackendName(DatabaseType type) noexcept;

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_TYPES_HPP_
