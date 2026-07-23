/// @file      error_code_layout.hpp
/// @brief     错误码分层编码格式
/// @author    wengjianhong2099@163.com
/// @date      2026-07-21
/// @copyright CC BY-NC-SA 4.0
#ifndef CPPUTILS_ERROR_CODE_ERROR_CODE_LAYOUT_HPP_
#define CPPUTILS_ERROR_CODE_ERROR_CODE_LAYOUT_HPP_

#include <cpputils/error_code/error_code_capacity.hpp>

#include <cstdint>

/// @brief 错误号四层分层分配说明：uint64_t 承载
/// @details 分层格式：AAA BBB CCC DDD
/// - 系统级编号 AAA：3位，取值 000~999
/// - 服务级编号 BBB：3位，取值 000~999
/// - 模块级编号 CCC：3位，取值 000~999
/// - 细分错误码 DDD：3位，取值 000~999
/// - 完整数值容量：000'000'000'000 ~ 999'999'999'999

namespace cpputils::error_code {

/// @brief 错误码拆分结果载体
struct ErrorCodeSegments {
  /// @brief 系统号 AAA
  uint64_t system_number;
  /// @brief 服务号 BBB
  uint64_t service_number;
  /// @brief 子模块号 CCC
  uint64_t module_number;
  /// @brief 错误号 DDD
  uint64_t code_number;
};

/// @brief 构造 AAABBB 格式服务编号
/// @param system_id AAA 级系统编号 [0,999]
/// @param service_number BBB 级服务编号 [0,999]
/// @return 组合后的服务ID，格式为 AAABBB
/// @throw std::out_of_range 当任一编号超出其允许范围
[[nodiscard]] inline constexpr uint64_t MakeServiceId(uint64_t system_id, uint64_t service_number);

/// @brief 构造 AAABBBCCC 格式模块编号
/// @param service_id AAABBB 级服务编号 [0,999999]
/// @param module_number CCC 级模块编号 [0,999]
/// @return 组合后的模块ID，格式为 AAABBBCCC
/// @throw std::out_of_range 当任一编号超出其允许范围
[[nodiscard]] inline constexpr uint64_t MakeModuleId(uint64_t service_id, uint64_t module_number);

/// @brief 构造 AAA BBB CCC DDD 格式完整错误码
/// @param module_id AAABBBCCC 级模块编号 [0,999999]
/// @param code_number DDD 级具体错误码 [0,999]
/// @return 组合后的完整错误码，格式为 AAA BBB CCC DDD
/// @throw std::out_of_range 当任一编号超出其允许范围
[[nodiscard]] inline constexpr uint64_t MakeErrorCode(uint64_t module_id, uint64_t code_number);

/// @brief 将完整错误码拆分为四层分段信息
[[nodiscard]] ErrorCodeSegments DecodeErrorCode(uint64_t code);

/// @brief 校验错误码整体格式、数值区间合法
[[nodiscard]] bool IsValidErrorCode(uint64_t code);

}  // namespace cpputils::error_code

// 引入内部内联实现；业务代码只应包含本公共头文件。
#include "detail/error_code_layout.inl"

#endif  // CPPUTILS_ERROR_CODE_ERROR_CODE_LAYOUT_HPP_
