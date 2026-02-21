cmake_minimum_required(VERSION 3.23)

project(EasyProfile VERSION 1.0.0 LANGUAGES CXX)

add_library(EasyProfile STATIC
	EasyObjectDictionary.cpp
	EasyProfile.cpp
	EasyProtocol.cpp
)

target_include_directories(EasyProfile
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include/EasyProfile>
)

set_target_properties(EasyProfile PROPERTIES
	PUBLIC_HEADER "EasyProfile.h;EasyObjectDictionary.h;EasyProtocol.h;BasicTypes.h;EasyQueue.h"
)

include(GNUInstallDirs)

install(TARGETS EasyProfile
	EXPORT EasyProfileTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/EasyProfile
)

install(EXPORT EasyProfileTargets
	FILE EasyProfileTargets.cmake
	NAMESPACE EasyProfile::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EasyProfile
)


