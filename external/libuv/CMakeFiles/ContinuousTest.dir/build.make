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

# Utility rule file for ContinuousTest.

# Include any custom commands dependencies for this target.
include external/libuv/CMakeFiles/ContinuousTest.dir/compiler_depend.make

# Include the progress variables for this target.
include external/libuv/CMakeFiles/ContinuousTest.dir/progress.make

external/libuv/CMakeFiles/ContinuousTest:
	cd /home/qz/code/libuv_test/external/libuv && /usr/local/cmake/bin/ctest -D ContinuousTest

ContinuousTest: external/libuv/CMakeFiles/ContinuousTest
ContinuousTest: external/libuv/CMakeFiles/ContinuousTest.dir/build.make
.PHONY : ContinuousTest

# Rule to build all files generated by this target.
external/libuv/CMakeFiles/ContinuousTest.dir/build: ContinuousTest
.PHONY : external/libuv/CMakeFiles/ContinuousTest.dir/build

external/libuv/CMakeFiles/ContinuousTest.dir/clean:
	cd /home/qz/code/libuv_test/external/libuv && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousTest.dir/cmake_clean.cmake
.PHONY : external/libuv/CMakeFiles/ContinuousTest.dir/clean

external/libuv/CMakeFiles/ContinuousTest.dir/depend:
	cd /home/qz/code/libuv_test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/qz/code/libuv_test /home/qz/code/libuv_test/external/libuv /home/qz/code/libuv_test /home/qz/code/libuv_test/external/libuv /home/qz/code/libuv_test/external/libuv/CMakeFiles/ContinuousTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/libuv/CMakeFiles/ContinuousTest.dir/depend
