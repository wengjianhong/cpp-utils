/// @file      database_types.hpp
/// @brief     数据库模块公共类型（DatabaseType 等）
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DATABASE_TYPES_HPP_
#define CPP_UTILS_DATABASE_DATABASE_TYPES_HPP_

#include <string_view>

namespace cpputils::database {

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
/// @return 类型名称：
/// - kUnknown: 空字符串
/// - kMySql: "mysql"
/// - kSqlite3: "sqlite3"
/// - kPostgreSql: "postgresql"
/// - kOracle: "oracle"
/// - kOdbc: "odbc"
[[nodiscard]] std::string_view GetDatabaseNameByType(DatabaseType type) noexcept;

/// @brief 从配置字符串解析 DatabaseType
/// @param name 类型名称：
/// - "mysql": kMySql
/// - "sqlite3": kSqlite3
/// - "postgresql": kPostgreSql
/// - "oracle": kOracle
/// - "odbc": kOdbc
/// - 未识别: kUnknown
[[nodiscard]] DatabaseType GetDatabaseTypeByName(std::string_view name) noexcept;

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_DATABASE_TYPES_HPP_
