# ---------------------------------------------------------------------------
# SOCI 可选依赖：find + 链接到 cpp-utils
# 需在 add_library(cpp-utils ...) 之后 include
# ---------------------------------------------------------------------------

# SOCI 可选依赖：find + 链接到 cpp-utils
option(CPPUTILS_ENABLE_SOCI "Build SOCI database wrapper" ON)

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
