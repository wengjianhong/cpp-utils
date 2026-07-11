cmake_minimum_required(VERSION 3.22)
project(cpputils VERSION 0.1.0 LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Toolchain defaults
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
set(CPPUTILS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE CPPUTILS_SOURCES CONFIGURE_DEPENDS
  "${CPPUTILS_SRC_DIR}/cpputils/*.cpp"
)

add_library(cpputils SHARED ${CPPUTILS_SOURCES})

# Public: #include <cpputils/...>
# Private: src/ as root; internal refs use "cpputils/..." (e.g. cpputils/database/...)
target_include_directories(cpputils
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE ${CPPUTILS_SRC_DIR}
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
