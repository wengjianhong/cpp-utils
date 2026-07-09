/// @file      types.hpp
/// @brief     数据库模块公共类型（DatabaseType 等）
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_TYPES_HPP_
#define CPP_UTILS_DATABASE_TYPES_HPP_

#include <string_view>

namespace cpp_utils::database {

/// @brief 支持的数据库类型
enum class DatabaseType {
  ///< 未知
  kUnknown = 0,
  ///< MySQL
  kMySql = 1,
  ///< SQLite3
  kSqlite3 = 2,
  ///< PostgreSQL
  kPostgreSql = 3,
  ///< Oracle
  kOracle = 4,
  ///< ODBC 通用驱动
  kOdbc = 5,
};

/// @brief 获取 DatabaseType 对应的名称字符串
/// @param type 数据库类型
/// @return 类型名称（如 "mysql"、"sqlite3"）；未知值返回空字符串
[[nodiscard]] std::string_view GetDatabaseTypeName(DatabaseType type) noexcept;

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_TYPES_HPP_
