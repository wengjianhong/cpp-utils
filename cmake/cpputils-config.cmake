# Entry point for find_package(cpputils CONFIG).
# Defines imported target: cpputils::cpputils

include(CMakeFindDependencyMacro)
find_dependency(Threads)

include("${CMAKE_CURRENT_LIST_DIR}/cpputilsTargets.cmake")
