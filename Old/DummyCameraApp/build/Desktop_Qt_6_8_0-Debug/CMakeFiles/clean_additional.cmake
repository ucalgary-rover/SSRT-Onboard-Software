# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appDummyCameraApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appDummyCameraApp_autogen.dir/ParseCache.txt"
  "appDummyCameraApp_autogen"
  )
endif()
