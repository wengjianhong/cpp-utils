/// @file      database_error.hpp
/// @brief     数据库错误详情（SOCI / 驱动原文，不含自定义错误码）
/// @details   cpputils 仅透传底层错误信息；成败由 API 返回值（bool / optional / nullptr）表达
/// @author    wengjianhong
/// @date      2026-07-09
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DATABASE_ERROR_HPP_
#define CPP_UTILS_DATABASE_DATABASE_ERROR_HPP_

#include <cpputils/database/database_types.hpp>

#include <string>

namespace cpputils::database {

/// @brief 数据库操作错误详情
struct DatabaseError {
  ///< 驱动原生错误码；0 表示无
  int native_code = 0;
  ///< 数据库类型
  DatabaseType database_type = DatabaseType::kUnknown;
  ///< 完整错误描述
  std::string message;
  ///< PostgreSQL 等数据库的标准错误码；空表示未提供
  std::string sqlstate;

  /// @brief 是否无错误
  /// @return true 表示 native_code 为 0 且 message、sqlstate 均为空
  [[nodiscard]] bool Ok() const noexcept;

  /// @brief 重置为无错误状态
  void Clear() noexcept;
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_DATABASE_ERROR_HPP_
