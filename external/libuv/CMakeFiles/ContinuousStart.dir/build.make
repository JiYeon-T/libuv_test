# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /usr/local/cmake/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/qz/code/libuv_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/qz/code/libuv_test

# Utility rule file for ContinuousStart.

# Include any custom commands dependencies for this target.
include external/libuv/CMakeFiles/ContinuousStart.dir/compiler_depend.make

# Include the progress variables for this target.
include external/libuv/CMakeFiles/ContinuousStart.dir/progress.make

external/libuv/CMakeFiles/ContinuousStart:
	cd /home/qz/code/libuv_test/external/libuv && /usr/local/cmake/bin/ctest -D ContinuousStart

ContinuousStart: external/libuv/CMakeFiles/ContinuousStart
ContinuousStart: external/libuv/CMakeFiles/ContinuousStart.dir/build.make
.PHONY : ContinuousStart

# Rule to build all files generated by this target.
external/libuv/CMakeFiles/ContinuousStart.dir/build: ContinuousStart
.PHONY : external/libuv/CMakeFiles/ContinuousStart.dir/build

external/libuv/CMakeFiles/ContinuousStart.dir/clean:
	cd /home/qz/code/libuv_test/external/libuv && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousStart.dir/cmake_clean.cmake
.PHONY : external/libuv/CMakeFiles/ContinuousStart.dir/clean

external/libuv/CMakeFiles/ContinuousStart.dir/depend:
	cd /home/qz/code/libuv_test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/qz/code/libuv_test /home/qz/code/libuv_test/external/libuv /home/qz/code/libuv_test /home/qz/code/libuv_test/external/libuv /home/qz/code/libuv_test/external/libuv/CMakeFiles/ContinuousStart.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/libuv/CMakeFiles/ContinuousStart.dir/depend

