# Install rules for find_package(cpputils CONFIG).
#
# Default prefix: /usr/local (override with -DCMAKE_INSTALL_PREFIX=).
# Installed layout:
#   lib/libcpputils.so
#   include/cpputils/...
#   lib/cmake/cpputils/cpputils-config.cmake
#   lib/cmake/cpputils/cpputils-config-version.cmake
#   lib/cmake/cpputils/cpputilsTargets.cmake

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cpputils-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install shared library; EXPORT registers the target for install(EXPORT) below
install(TARGETS cpputils EXPORT cpputilsTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  # Explicit 755 to avoid link/load failures from restrictive .so permissions
  PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

# Install public headers; downstream uses #include <cpputils/...>
install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Generate and install Targets file; NAMESPACE exposes cpputils::cpputils
install(EXPORT cpputilsTargets
  FILE cpputilsTargets.cmake
  NAMESPACE cpputils::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpputils
)

# Package config: includes cpputilsTargets.cmake
install(FILES
  ${CMAKE_CURRENT_LIST_DIR}/cpputils-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cpputils-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpputils
)
