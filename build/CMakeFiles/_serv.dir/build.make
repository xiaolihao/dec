# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/local/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tll007/dev/dec

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tll007/dev/dec/build

# Include any dependencies generated for this target.
include CMakeFiles/_serv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/_serv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/_serv.dir/flags.make

CMakeFiles/_serv.dir/src/_serv.c.o: CMakeFiles/_serv.dir/flags.make
CMakeFiles/_serv.dir/src/_serv.c.o: ../src/_serv.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/tll007/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/_serv.dir/src/_serv.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/_serv.dir/src/_serv.c.o   -c /Users/tll007/dev/dec/src/_serv.c

CMakeFiles/_serv.dir/src/_serv.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/_serv.dir/src/_serv.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /Users/tll007/dev/dec/src/_serv.c > CMakeFiles/_serv.dir/src/_serv.c.i

CMakeFiles/_serv.dir/src/_serv.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/_serv.dir/src/_serv.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /Users/tll007/dev/dec/src/_serv.c -o CMakeFiles/_serv.dir/src/_serv.c.s

CMakeFiles/_serv.dir/src/_serv.c.o.requires:
.PHONY : CMakeFiles/_serv.dir/src/_serv.c.o.requires

CMakeFiles/_serv.dir/src/_serv.c.o.provides: CMakeFiles/_serv.dir/src/_serv.c.o.requires
	$(MAKE) -f CMakeFiles/_serv.dir/build.make CMakeFiles/_serv.dir/src/_serv.c.o.provides.build
.PHONY : CMakeFiles/_serv.dir/src/_serv.c.o.provides

CMakeFiles/_serv.dir/src/_serv.c.o.provides.build: CMakeFiles/_serv.dir/src/_serv.c.o

# Object files for target _serv
_serv_OBJECTS = \
"CMakeFiles/_serv.dir/src/_serv.c.o"

# External object files for target _serv
_serv_EXTERNAL_OBJECTS =

_serv: CMakeFiles/_serv.dir/src/_serv.c.o
_serv: CMakeFiles/_serv.dir/build.make
_serv: libdec.a
_serv: CMakeFiles/_serv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable _serv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/_serv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/_serv.dir/build: _serv
.PHONY : CMakeFiles/_serv.dir/build

CMakeFiles/_serv.dir/requires: CMakeFiles/_serv.dir/src/_serv.c.o.requires
.PHONY : CMakeFiles/_serv.dir/requires

CMakeFiles/_serv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/_serv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/_serv.dir/clean

CMakeFiles/_serv.dir/depend:
	cd /Users/tll007/dev/dec/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tll007/dev/dec /Users/tll007/dev/dec /Users/tll007/dev/dec/build /Users/tll007/dev/dec/build /Users/tll007/dev/dec/build/CMakeFiles/_serv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/_serv.dir/depend

