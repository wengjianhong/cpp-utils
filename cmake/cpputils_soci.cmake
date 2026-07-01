# Optional SOCI dependency for the database wrapper.
# Include after add_library(cpputils ...); 
# toggle via CPPUTILS_ENABLE_SOCI in root CMakeLists.txt.
# Disable: cmake -DCPPUTILS_ENABLE_SOCI=OFF

if(NOT CPPUTILS_ENABLE_SOCI)
  return()
endif()

# soci_core is required; backend drivers are linked only when found (tests use sqlite3)
find_library(SOCI_CORE_LIB soci_core REQUIRED)
find_library(SOCI_SQLITE3_LIB soci_sqlite3)
find_library(SOCI_MYSQL_LIB soci_mysql)
find_library(SOCI_POSTGRESQL_LIB soci_postgresql)

# dl: SOCI loads backend .so at runtime based on the connection string
target_link_libraries(cpputils PUBLIC ${SOCI_CORE_LIB} dl)

# Link only backends found on this machine; a missing driver does not fail the build
foreach(_backend IN ITEMS SOCI_SQLITE3_LIB SOCI_MYSQL_LIB SOCI_POSTGRESQL_LIB)
  if(${_backend})
    target_link_libraries(cpputils PUBLIC ${${_backend}})
  endif()
endforeach()
