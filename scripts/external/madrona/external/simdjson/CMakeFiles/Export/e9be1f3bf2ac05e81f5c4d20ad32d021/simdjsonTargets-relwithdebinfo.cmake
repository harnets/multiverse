#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "simdjson::simdjson" for configuration "RelWithDebInfo"
set_property(TARGET simdjson::simdjson APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(simdjson::simdjson PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libsimdjson.a"
  )

list(APPEND _cmake_import_check_targets simdjson::simdjson )
list(APPEND _cmake_import_check_files_for_simdjson::simdjson "${_IMPORT_PREFIX}/lib/libsimdjson.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
