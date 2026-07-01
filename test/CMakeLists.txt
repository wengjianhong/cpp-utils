# GTest-based unit tests (aligned with parent project conventions)
find_package(GTest REQUIRED CONFIG)

add_executable(cpputils_tests
    test_safe_queue.cpp
    test_thread_pool.cpp
    test_database.cpp
    test_config.cpp
)

# 通过链接 cpputils 获得 include 路径

# Find dependencies
find_package(GTest 1.16.1 EXACT REQUIRED)

# Link dependencies
target_link_libraries(cpputils_tests PRIVATE
    cpputils
    GTest::gtest
    GTest::gtest_main
)

# Discover and run tests
include(GoogleTest)
gtest_discover_tests(cpputils_tests)
