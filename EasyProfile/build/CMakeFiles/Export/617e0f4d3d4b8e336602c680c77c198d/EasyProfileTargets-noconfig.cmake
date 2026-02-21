#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EasyProfile::EasyProfile" for configuration ""
set_property(TARGET EasyProfile::EasyProfile APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(EasyProfile::EasyProfile PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libEasyProfile.a"
  )

list(APPEND _cmake_import_check_targets EasyProfile::EasyProfile )
list(APPEND _cmake_import_check_files_for_EasyProfile::EasyProfile "${_IMPORT_PREFIX}/lib/libEasyProfile.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
