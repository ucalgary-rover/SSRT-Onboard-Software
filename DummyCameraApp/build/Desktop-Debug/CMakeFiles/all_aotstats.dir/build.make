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

# Utility rule file for all_aotstats.

# Include any custom commands dependencies for this target.
include CMakeFiles/all_aotstats.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/all_aotstats.dir/progress.make

CMakeFiles/all_aotstats: .rcc/qmlcache/all_aotstats.txt
	/efi/@/usr/bin/cmake -E cat /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/.rcc/qmlcache/all_aotstats.txt

.rcc/qmlcache/all_aotstats.aotstats:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating .rcc/qmlcache/all_aotstats.aotstats, .rcc/qmlcache/all_aotstats.txt"
	/usr/lib/qt6/qmlaotstats aggregate /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/.rcc/qmlcache/all_aotstats.aotstatslist /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/.rcc/qmlcache/all_aotstats.aotstats
	/usr/lib/qt6/qmlaotstats format /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/.rcc/qmlcache/all_aotstats.aotstats /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/.rcc/qmlcache/all_aotstats.txt

.rcc/qmlcache/all_aotstats.txt: .rcc/qmlcache/all_aotstats.aotstats
	@$(CMAKE_COMMAND) -E touch_nocreate .rcc/qmlcache/all_aotstats.txt

CMakeFiles/all_aotstats.dir/codegen:
.PHONY : CMakeFiles/all_aotstats.dir/codegen

all_aotstats: .rcc/qmlcache/all_aotstats.aotstats
all_aotstats: .rcc/qmlcache/all_aotstats.txt
all_aotstats: CMakeFiles/all_aotstats
all_aotstats: CMakeFiles/all_aotstats.dir/build.make
.PHONY : all_aotstats

# Rule to build all files generated by this target.
CMakeFiles/all_aotstats.dir/build: all_aotstats
.PHONY : CMakeFiles/all_aotstats.dir/build

CMakeFiles/all_aotstats.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/all_aotstats.dir/cmake_clean.cmake
.PHONY : CMakeFiles/all_aotstats.dir/clean

CMakeFiles/all_aotstats.dir/depend:
	cd /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug /home/tymarc/Software/2024-2025-Software/base-development/DummyCameraApp/build/Desktop-Debug/CMakeFiles/all_aotstats.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/all_aotstats.dir/depend

