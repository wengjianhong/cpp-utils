/// @file      error_code_capacity.hpp
/// @brief     错误码分层编码容量
/// @author    wengjianhong2099@163.com
/// @date      2026-07-21
/// @copyright CC BY-NC-SA 4.0
#ifndef CPPUTILS_ERROR_CODE_ERROR_CODE_CAPACITY_HPP_
#define CPPUTILS_ERROR_CODE_ERROR_CODE_CAPACITY_HPP_

#include <cstdint>

namespace cpputils::error_code {

/// 系统级编号段容量，[0,999]
constexpr uint64_t kSystemNumberCapacity = 1000;
/// 服务级编号段容量，[0,999]
constexpr uint64_t kServiceNumberCapacity = 1000;
/// 模块级编号段容量，[0,999]
constexpr uint64_t kModuleNumberCapacity = 1000;
/// 错误码编号段容量，[0,999]
constexpr uint64_t kCodeNumberCapacity = 1000;

}  // namespace cpputils::error_code

#endif  // CPPUTILS_ERROR_CODE_ERROR_CODE_CAPACITY_HPP_
