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
include CMakeFiles/_dec.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/_dec.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/_dec.dir/flags.make

CMakeFiles/_dec.dir/src/_dec.c.o: CMakeFiles/_dec.dir/flags.make
CMakeFiles/_dec.dir/src/_dec.c.o: ../src/_dec.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/tll007/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/_dec.dir/src/_dec.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/_dec.dir/src/_dec.c.o   -c /Users/tll007/dev/dec/src/_dec.c

CMakeFiles/_dec.dir/src/_dec.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/_dec.dir/src/_dec.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /Users/tll007/dev/dec/src/_dec.c > CMakeFiles/_dec.dir/src/_dec.c.i

CMakeFiles/_dec.dir/src/_dec.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/_dec.dir/src/_dec.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /Users/tll007/dev/dec/src/_dec.c -o CMakeFiles/_dec.dir/src/_dec.c.s

CMakeFiles/_dec.dir/src/_dec.c.o.requires:
.PHONY : CMakeFiles/_dec.dir/src/_dec.c.o.requires

CMakeFiles/_dec.dir/src/_dec.c.o.provides: CMakeFiles/_dec.dir/src/_dec.c.o.requires
	$(MAKE) -f CMakeFiles/_dec.dir/build.make CMakeFiles/_dec.dir/src/_dec.c.o.provides.build
.PHONY : CMakeFiles/_dec.dir/src/_dec.c.o.provides

CMakeFiles/_dec.dir/src/_dec.c.o.provides.build: CMakeFiles/_dec.dir/src/_dec.c.o

# Object files for target _dec
_dec_OBJECTS = \
"CMakeFiles/_dec.dir/src/_dec.c.o"

# External object files for target _dec
_dec_EXTERNAL_OBJECTS =

_dec: CMakeFiles/_dec.dir/src/_dec.c.o
_dec: CMakeFiles/_dec.dir/build.make
_dec: libdec.a
_dec: CMakeFiles/_dec.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable _dec"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/_dec.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/_dec.dir/build: _dec
.PHONY : CMakeFiles/_dec.dir/build

CMakeFiles/_dec.dir/requires: CMakeFiles/_dec.dir/src/_dec.c.o.requires
.PHONY : CMakeFiles/_dec.dir/requires

CMakeFiles/_dec.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/_dec.dir/cmake_clean.cmake
.PHONY : CMakeFiles/_dec.dir/clean

CMakeFiles/_dec.dir/depend:
	cd /Users/tll007/dev/dec/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tll007/dev/dec /Users/tll007/dev/dec /Users/tll007/dev/dec/build /Users/tll007/dev/dec/build /Users/tll007/dev/dec/build/CMakeFiles/_dec.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/_dec.dir/depend

