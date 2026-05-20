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
add_compile_options( -fPIC -Wall -Wextra -Werror -O2)

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(ALL_SRC_FILES 
    ${SRC_FILES}
)

# Create shared library
add_library(cpp-utils SHARED ${ALL_SRC_FILES})

# Include directories
target_include_directories(cpp-utils PUBLIC ${CMAKE_SOURCE_DIR})

# Find dependencies
find_package(Threads REQUIRED)
#find_package(spdlog 1.14.1 EXACT REQUIRED)

# Link dependencies
target_link_libraries(cpp-utils PUBLIC Threads::Threads)
#target_link_libraries(cpp-utils PUBLIC spdlog::spdlog)

# Build output directories (clean and standard layout)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_SOURCE_DIR}/release/include)
# Install the library
install(TARGETS cpp-utils LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/release/lib)

# Build test cases
add_subdirectory(test)
