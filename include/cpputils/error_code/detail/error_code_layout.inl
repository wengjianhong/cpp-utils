/// @file      error_code_layout.inl
/// @brief     错误码编译期构造工具内联实现
/// @warning   内部实现文件，禁止业务代码直接 #include

#include "cpputils/error_code/error_code_capacity.hpp"

#include <cstdint>
#include <stdexcept>

namespace cpputils::error_code {
namespace internal {

/// @brief 系统级ID段范围:[0, 1000)
constexpr uint64_t kSystemIdCapacity = kSystemNumberCapacity;
/// @brief 服务级ID段范围:[0, 1000'000)
constexpr uint64_t kServiceIdCapacity = kServiceNumberCapacity * kSystemIdCapacity;
/// @brief 模块级ID段范围:[0, 1'000'000'000)
constexpr uint64_t kModuleIdCapacity = kModuleNumberCapacity * kServiceIdCapacity;
/// @brief 具体错误码ID段范围:[0, 1'000'000'000'000)
constexpr uint64_t kCodeIdCapacity = kCodeNumberCapacity * kModuleIdCapacity;

inline constexpr void ValidateSegmentNumber(uint64_t number, uint64_t capacity, const char* info) {
  if (number >= capacity) {
    throw std::out_of_range(info);
  }
}

}  // namespace internal

inline constexpr uint64_t MakeServiceId(uint64_t system_id, uint64_t service_number) {
  internal::ValidateSegmentNumber(system_id, internal::kSystemIdCapacity, "system id out of range [0,999]");
  internal::ValidateSegmentNumber(service_number, kServiceNumberCapacity, "service id out of range [0,999]");

  return system_id * kServiceNumberCapacity + service_number;
}

inline constexpr uint64_t MakeModuleId(uint64_t service_id, uint64_t module_number) {
  internal::ValidateSegmentNumber(service_id, internal::kServiceIdCapacity, "service id out of range [0,999999]");
  internal::ValidateSegmentNumber(module_number, kModuleNumberCapacity, "module number out of range [0,999]");

  return service_id * kModuleNumberCapacity + module_number;
}

inline constexpr uint64_t MakeErrorCode(uint64_t module_id, uint64_t code_number) {
  internal::ValidateSegmentNumber(module_id, internal::kModuleIdCapacity, "module id out of range [0,999999999]");
  internal::ValidateSegmentNumber(code_number, kCodeNumberCapacity, "detail code out of range [0,999]");

  return module_id * kCodeNumberCapacity + code_number;
}

}  // namespace cpputils::error_code
