/// @file      error.hpp
/// @brief     数据库操作结果码
/// @details   与 SOCI / 具体驱动解耦，供 Connection 与 Pool 统一返回
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_ERROR_HPP_
#define CPP_UTILS_DATABASE_ERROR_HPP_

namespace cpp_utils::database {

/// @brief 数据库操作结果码
enum class Error {
  kSuccess = 0,            ///< 成功
  kInvalidArgument = 1,    ///< 参数非法
  kNotConnected = 2,       ///< 未连接
  kConnectFailed = 3,      ///< 连接失败
  kQueryFailed = 4,        ///< 查询失败
  kExecuteFailed = 5,      ///< 执行失败
  kTransactionFailed = 6,  ///< 事务提交/回滚失败
  kNotFound = 7,           ///< 资源不存在
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_ERROR_HPP_
