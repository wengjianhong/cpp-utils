#include "cpputils/error_code/error_code_layout.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace cpputils::error_code {
namespace {

static_assert(MakeServiceId(12, 34) == 12034);
static_assert(MakeModuleId(MakeServiceId(12, 34), 56) == 12034056);
static_assert(MakeErrorCode(MakeModuleId(MakeServiceId(12, 34), 56), 78) == 12034056078);

TEST(ErrorCodeLayoutTest, DecodesEverySegment) {
  constexpr uint64_t kServiceId = MakeServiceId(12, 34);
  constexpr uint64_t kModuleId = MakeModuleId(kServiceId, 56);
  constexpr uint64_t kCode = MakeErrorCode(kModuleId, 78);

  const auto segments = DecodeErrorCode(kCode);
  EXPECT_EQ(segments.system_number, 12);
  EXPECT_EQ(segments.service_number, 34);
  EXPECT_EQ(segments.module_number, 56);
  EXPECT_EQ(segments.code_number, 78);
}

TEST(ErrorCodeLayoutTest, ValidatesCodeRange) {
  EXPECT_TRUE(IsValidErrorCode(0));
  EXPECT_TRUE(IsValidErrorCode(MakeErrorCode(MakeModuleId(MakeServiceId(999, 999), 999), 999)));
  EXPECT_FALSE(IsValidErrorCode(1000000000000));
}

TEST(ErrorCodeLayoutTest, RejectsInvalidSegmentNumbers) {
  EXPECT_THROW(static_cast<void>(MakeServiceId(1000, 0)), std::out_of_range);
  EXPECT_THROW(static_cast<void>(MakeModuleId(1000000, 0)), std::out_of_range);
  EXPECT_THROW(static_cast<void>(MakeModuleId(1, 1000)), std::out_of_range);
  EXPECT_THROW(static_cast<void>(MakeErrorCode(1000000000, 0)), std::out_of_range);
  EXPECT_THROW(static_cast<void>(MakeErrorCode(1, 1000)), std::out_of_range);
}

}  // namespace
}  // namespace cpputils::error_code
