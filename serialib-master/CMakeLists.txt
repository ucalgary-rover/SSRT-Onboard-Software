
cmake_minimum_required(VERSION 3.23)

project(serialib VERSION 1.0.0 LANGUAGES CXX)

add_library(serialib STATIC
	serialib.cpp
)

target_include_directories(serialib
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include/serialib>
)

set_target_properties(serialib PROPERTIES
	PUBLIC_HEADER "serialib.h"
)

include(GNUInstallDirs)

install(TARGETS serialib
	EXPORT serialibTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/serialib
)

install(EXPORT serialibTargets
	FILE serialibTargets.cmake
	NAMESPACE serialib::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/serialib
)







