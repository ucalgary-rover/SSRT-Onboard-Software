# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /efi/@/usr/bin/cmake

# The command to remove a file.
RM = /efi/@/usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug

# Utility rule file for appDummyCameraApp_tooling.

# Include any custom commands dependencies for this target.
include CMakeFiles/appDummyCameraApp_tooling.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/appDummyCameraApp_tooling.dir/progress.make

DummyCameraApp/Main.qml: /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/Main.qml
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Copying Main.qml to /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/DummyCameraApp/Main.qml"
	cd /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp && /efi/@/usr/bin/cmake -E copy /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/Main.qml /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/DummyCameraApp/Main.qml

CMakeFiles/appDummyCameraApp_tooling.dir/codegen:
.PHONY : CMakeFiles/appDummyCameraApp_tooling.dir/codegen

appDummyCameraApp_tooling: DummyCameraApp/Main.qml
appDummyCameraApp_tooling: CMakeFiles/appDummyCameraApp_tooling.dir/build.make
.PHONY : appDummyCameraApp_tooling

# Rule to build all files generated by this target.
CMakeFiles/appDummyCameraApp_tooling.dir/build: appDummyCameraApp_tooling
.PHONY : CMakeFiles/appDummyCameraApp_tooling.dir/build

CMakeFiles/appDummyCameraApp_tooling.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/appDummyCameraApp_tooling.dir/cmake_clean.cmake
.PHONY : CMakeFiles/appDummyCameraApp_tooling.dir/clean

CMakeFiles/appDummyCameraApp_tooling.dir/depend:
	cd /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/CMakeFiles/appDummyCameraApp_tooling.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/appDummyCameraApp_tooling.dir/depend

