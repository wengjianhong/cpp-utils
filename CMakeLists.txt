cmake_minimum_required(VERSION 3.22)
project(cpputils LANGUAGES CXX)


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

# Collect all source files under src/ (recursive)
file(GLOB_RECURSE ALL_SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create shared library
add_library(cpputils SHARED ${ALL_SRC_FILES})

# include：<cpputils/...>；src/ 下 detail 头文件仅编译期使用
target_include_directories(cpputils
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

# Find dependencies
find_package(Threads REQUIRED)
#find_package(spdlog 1.14.1 EXACT REQUIRED)

# Link dependencies
target_link_libraries(cpputils PUBLIC Threads::Threads)
#target_link_libraries(cpputils PUBLIC spdlog::spdlog)

# SOCI database dependencies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpputils_soci.cmake)

# Build output directories (clean and standard layout)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install（默认 prefix 如 /usr/local），可用 -DCMAKE_INSTALL_PREFIX= 覆盖
include(GNUInstallDirs)
install(TARGETS cpputils
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Build test cases
enable_testing()
add_subdirectory(test)
