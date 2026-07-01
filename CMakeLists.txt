cmake_minimum_required(VERSION 3.22)
project(cpputils VERSION 0.1.0 LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Toolchain defaults
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_compile_options(-fPIC -Wall -Wextra -Werror -O2)

# -----------------------------------------------------------------------------
# Options
# -----------------------------------------------------------------------------
option(CPPUTILS_ENABLE_SOCI "Build SOCI database wrapper" ON)

# -----------------------------------------------------------------------------
# Library
# -----------------------------------------------------------------------------
file(GLOB_RECURSE CPPUTILS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(cpputils SHARED ${CPPUTILS_SOURCES})

# Public: #include <cpputils/...>
# Private: src/ detail headers, compile-time only for this target
target_include_directories(cpputils
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Threads REQUIRED)
target_link_libraries(cpputils PUBLIC Threads::Threads)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpputils_soci.cmake)

# -----------------------------------------------------------------------------
# Install (downstream: find_package(cpputils CONFIG))
# -----------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpputils_install.cmake)

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
enable_testing()
add_subdirectory(test)
