cmake_minimum_required(VERSION 3.22)
project(cpp-utils LANGUAGES CXX)

# C++ Standard configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Standard compiler flags for production-grade shared library
add_compile_options(-fPIC -Wall -Wextra -Werror -O2)

option(CPPUTILS_ENABLE_SOCI "Build SOCI database wrapper" ON)

# Collect all source files (include nested database/detail)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.cpp"
)
set(ALL_SRC_FILES ${SRC_FILES})

# Create shared library
add_library(cpp-utils SHARED ${ALL_SRC_FILES})

# include directories
target_include_directories(cpp-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Find dependencies
find_package(Threads REQUIRED)
#find_package(spdlog 1.14.1 EXACT REQUIRED)

# Link dependencies
target_link_libraries(cpp-utils PUBLIC Threads::Threads)
#target_link_libraries(cpp-utils PUBLIC spdlog::spdlog)

if(CPPUTILS_ENABLE_SOCI)
    find_library(SOCI_CORE_LIB soci_core REQUIRED)
    find_library(SOCI_SQLITE3_LIB soci_sqlite3)
    find_library(SOCI_MYSQL_LIB soci_mysql)
    find_library(SOCI_POSTGRESQL_LIB soci_postgresql)

    target_compile_definitions(cpp-utils PUBLIC CPPUTILS_WITH_SOCI=1)
    target_link_libraries(cpp-utils PUBLIC ${SOCI_CORE_LIB} dl)

    if(SOCI_SQLITE3_LIB)
        target_link_libraries(cpp-utils PUBLIC ${SOCI_SQLITE3_LIB})
    endif()
    if(SOCI_MYSQL_LIB)
        target_link_libraries(cpp-utils PUBLIC ${SOCI_MYSQL_LIB})
    endif()
    if(SOCI_POSTGRESQL_LIB)
        target_link_libraries(cpp-utils PUBLIC ${SOCI_POSTGRESQL_LIB})
    endif()
endif()

# Build output directories (clean and standard layout)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/release/include)
# Install the library
install(TARGETS cpp-utils LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/release/lib)

# Build test cases
enable_testing()
add_subdirectory(test)
