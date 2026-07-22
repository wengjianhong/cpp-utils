#include "cpputils/error_code/error_code_layout.hpp"

namespace cpputils::error_code {

/// @brief 模块级编号段放大倍率: 1000
constexpr uint64_t kModuleIdScale = kCodeNumberCapacity;
/// @brief 服务级编号段放大倍率: 1000'000
constexpr uint64_t kServiceIdScale = kModuleNumberCapacity * kModuleIdScale;
/// @brief 系统级编号段放大倍率: 1'000'000'000
constexpr uint64_t kSystemIdScale = kServiceNumberCapacity * kServiceIdScale;

ErrorCodeSegments DecodeErrorCode(uint64_t code) {
  ErrorCodeSegments segments{
    .system_number = code / kSystemIdScale,
    .service_number = (code / kServiceIdScale) % kServiceNumberCapacity,
    .module_number = (code / kModuleIdScale) % kModuleNumberCapacity,
    .code_number = code % kCodeNumberCapacity,
  };
  return segments;
}

bool IsValidErrorCode(uint64_t code) {
  return code < internal::kCodeIdCapacity;
}

}  // namespace cpputils::error_code
