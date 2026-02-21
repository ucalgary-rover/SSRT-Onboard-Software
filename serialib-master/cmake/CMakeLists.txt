# CMake export file for the serialib library.

set(package_name serialib)

configure_file(${package_name}Config.cmake.in ${package_name}Config.cmake @ONLY)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${package_name}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

export(EXPORT ${package_name}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${package_name}Targets.cmake"
  NAMESPACE ${package_name}::
)

include(GNUInstallDirs)

install(EXPORT ${package_name}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${package_name}
  FILE ${package_name}Targets.cmake
  NAMESPACE ${package_name}::
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${package_name}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${package_name}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${package_name}
)
