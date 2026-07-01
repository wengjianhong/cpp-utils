# GTest unit tests for cpputils

# Dependencies (minimum 1.14.0, aligned with qtrade)
find_package(GTest 1.14.0 REQUIRED)

# Source files
file(GLOB_RECURSE CPPUTILS_TEST_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp"
)

# Build executable
add_executable(cpputils_tests ${CPPUTILS_TEST_SOURCES})

# Link libraries (include paths propagated from cpputils)
target_link_libraries(cpputils_tests PRIVATE
  cpputils
  GTest::gtest
  GTest::gtest_main
)

# Discover and run tests
include(GoogleTest)
gtest_discover_tests(cpputils_tests)
