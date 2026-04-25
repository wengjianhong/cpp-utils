# GTest-based unit tests (aligned with parent project conventions)
find_package(GTest REQUIRED CONFIG)

add_executable(cpp_utils_tests
    test_safe_queue.cpp
    test_thread_pool.cpp
)

# Same include layout as root: headers live under include/ (e.g. include/safe_queue.hh)
target_include_directories(cpp_utils_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Find dependencies
find_package(GTest 1.16.1 EXACT REQUIRED)

# Link dependencies
target_link_libraries(cpp_utils_tests PRIVATE
    cpp-utils
    GTest::gtest
    GTest::gtest_main
)

# Discover and run tests
include(GoogleTest)
gtest_discover_tests(cpp_utils_tests)
