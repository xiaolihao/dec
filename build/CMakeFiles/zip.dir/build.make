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
CMAKE_SOURCE_DIR = /Users/xiao/dev/dec

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/xiao/dev/dec/build

# Include any dependencies generated for this target.
include CMakeFiles/zip.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zip.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zip.dir/flags.make

CMakeFiles/zip.dir/minizip/src/ioapi.c.o: CMakeFiles/zip.dir/flags.make
CMakeFiles/zip.dir/minizip/src/ioapi.c.o: ../minizip/src/ioapi.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/xiao/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/zip.dir/minizip/src/ioapi.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/zip.dir/minizip/src/ioapi.c.o   -c /Users/xiao/dev/dec/minizip/src/ioapi.c

CMakeFiles/zip.dir/minizip/src/ioapi.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zip.dir/minizip/src/ioapi.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /Users/xiao/dev/dec/minizip/src/ioapi.c > CMakeFiles/zip.dir/minizip/src/ioapi.c.i

CMakeFiles/zip.dir/minizip/src/ioapi.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zip.dir/minizip/src/ioapi.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /Users/xiao/dev/dec/minizip/src/ioapi.c -o CMakeFiles/zip.dir/minizip/src/ioapi.c.s

CMakeFiles/zip.dir/minizip/src/ioapi.c.o.requires:
.PHONY : CMakeFiles/zip.dir/minizip/src/ioapi.c.o.requires

CMakeFiles/zip.dir/minizip/src/ioapi.c.o.provides: CMakeFiles/zip.dir/minizip/src/ioapi.c.o.requires
	$(MAKE) -f CMakeFiles/zip.dir/build.make CMakeFiles/zip.dir/minizip/src/ioapi.c.o.provides.build
.PHONY : CMakeFiles/zip.dir/minizip/src/ioapi.c.o.provides

CMakeFiles/zip.dir/minizip/src/ioapi.c.o.provides.build: CMakeFiles/zip.dir/minizip/src/ioapi.c.o

CMakeFiles/zip.dir/minizip/src/zip.c.o: CMakeFiles/zip.dir/flags.make
CMakeFiles/zip.dir/minizip/src/zip.c.o: ../minizip/src/zip.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/xiao/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/zip.dir/minizip/src/zip.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/zip.dir/minizip/src/zip.c.o   -c /Users/xiao/dev/dec/minizip/src/zip.c

CMakeFiles/zip.dir/minizip/src/zip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zip.dir/minizip/src/zip.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /Users/xiao/dev/dec/minizip/src/zip.c > CMakeFiles/zip.dir/minizip/src/zip.c.i

CMakeFiles/zip.dir/minizip/src/zip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zip.dir/minizip/src/zip.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /Users/xiao/dev/dec/minizip/src/zip.c -o CMakeFiles/zip.dir/minizip/src/zip.c.s

CMakeFiles/zip.dir/minizip/src/zip.c.o.requires:
.PHONY : CMakeFiles/zip.dir/minizip/src/zip.c.o.requires

CMakeFiles/zip.dir/minizip/src/zip.c.o.provides: CMakeFiles/zip.dir/minizip/src/zip.c.o.requires
	$(MAKE) -f CMakeFiles/zip.dir/build.make CMakeFiles/zip.dir/minizip/src/zip.c.o.provides.build
.PHONY : CMakeFiles/zip.dir/minizip/src/zip.c.o.provides

CMakeFiles/zip.dir/minizip/src/zip.c.o.provides.build: CMakeFiles/zip.dir/minizip/src/zip.c.o

CMakeFiles/zip.dir/minizip/src/unzip.c.o: CMakeFiles/zip.dir/flags.make
CMakeFiles/zip.dir/minizip/src/unzip.c.o: ../minizip/src/unzip.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/xiao/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/zip.dir/minizip/src/unzip.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/zip.dir/minizip/src/unzip.c.o   -c /Users/xiao/dev/dec/minizip/src/unzip.c

CMakeFiles/zip.dir/minizip/src/unzip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zip.dir/minizip/src/unzip.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /Users/xiao/dev/dec/minizip/src/unzip.c > CMakeFiles/zip.dir/minizip/src/unzip.c.i

CMakeFiles/zip.dir/minizip/src/unzip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zip.dir/minizip/src/unzip.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /Users/xiao/dev/dec/minizip/src/unzip.c -o CMakeFiles/zip.dir/minizip/src/unzip.c.s

CMakeFiles/zip.dir/minizip/src/unzip.c.o.requires:
.PHONY : CMakeFiles/zip.dir/minizip/src/unzip.c.o.requires

CMakeFiles/zip.dir/minizip/src/unzip.c.o.provides: CMakeFiles/zip.dir/minizip/src/unzip.c.o.requires
	$(MAKE) -f CMakeFiles/zip.dir/build.make CMakeFiles/zip.dir/minizip/src/unzip.c.o.provides.build
.PHONY : CMakeFiles/zip.dir/minizip/src/unzip.c.o.provides

CMakeFiles/zip.dir/minizip/src/unzip.c.o.provides.build: CMakeFiles/zip.dir/minizip/src/unzip.c.o

CMakeFiles/zip.dir/minizip/src/mztools.c.o: CMakeFiles/zip.dir/flags.make
CMakeFiles/zip.dir/minizip/src/mztools.c.o: ../minizip/src/mztools.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/xiao/dev/dec/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/zip.dir/minizip/src/mztools.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/zip.dir/minizip/src/mztools.c.o   -c /Users/xiao/dev/dec/minizip/src/mztools.c

CMakeFiles/zip.dir/minizip/src/mztools.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/zip.dir/minizip/src/mztools.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /Users/xiao/dev/dec/minizip/src/mztools.c > CMakeFiles/zip.dir/minizip/src/mztools.c.i

CMakeFiles/zip.dir/minizip/src/mztools.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/zip.dir/minizip/src/mztools.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /Users/xiao/dev/dec/minizip/src/mztools.c -o CMakeFiles/zip.dir/minizip/src/mztools.c.s

CMakeFiles/zip.dir/minizip/src/mztools.c.o.requires:
.PHONY : CMakeFiles/zip.dir/minizip/src/mztools.c.o.requires

CMakeFiles/zip.dir/minizip/src/mztools.c.o.provides: CMakeFiles/zip.dir/minizip/src/mztools.c.o.requires
	$(MAKE) -f CMakeFiles/zip.dir/build.make CMakeFiles/zip.dir/minizip/src/mztools.c.o.provides.build
.PHONY : CMakeFiles/zip.dir/minizip/src/mztools.c.o.provides

CMakeFiles/zip.dir/minizip/src/mztools.c.o.provides.build: CMakeFiles/zip.dir/minizip/src/mztools.c.o

# Object files for target zip
zip_OBJECTS = \
"CMakeFiles/zip.dir/minizip/src/ioapi.c.o" \
"CMakeFiles/zip.dir/minizip/src/zip.c.o" \
"CMakeFiles/zip.dir/minizip/src/unzip.c.o" \
"CMakeFiles/zip.dir/minizip/src/mztools.c.o"

# External object files for target zip
zip_EXTERNAL_OBJECTS =

libzip.a: CMakeFiles/zip.dir/minizip/src/ioapi.c.o
libzip.a: CMakeFiles/zip.dir/minizip/src/zip.c.o
libzip.a: CMakeFiles/zip.dir/minizip/src/unzip.c.o
libzip.a: CMakeFiles/zip.dir/minizip/src/mztools.c.o
libzip.a: CMakeFiles/zip.dir/build.make
libzip.a: CMakeFiles/zip.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libzip.a"
	$(CMAKE_COMMAND) -P CMakeFiles/zip.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zip.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zip.dir/build: libzip.a
.PHONY : CMakeFiles/zip.dir/build

CMakeFiles/zip.dir/requires: CMakeFiles/zip.dir/minizip/src/ioapi.c.o.requires
CMakeFiles/zip.dir/requires: CMakeFiles/zip.dir/minizip/src/zip.c.o.requires
CMakeFiles/zip.dir/requires: CMakeFiles/zip.dir/minizip/src/unzip.c.o.requires
CMakeFiles/zip.dir/requires: CMakeFiles/zip.dir/minizip/src/mztools.c.o.requires
.PHONY : CMakeFiles/zip.dir/requires

CMakeFiles/zip.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zip.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zip.dir/clean

CMakeFiles/zip.dir/depend:
	cd /Users/xiao/dev/dec/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/xiao/dev/dec /Users/xiao/dev/dec /Users/xiao/dev/dec/build /Users/xiao/dev/dec/build /Users/xiao/dev/dec/build/CMakeFiles/zip.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zip.dir/depend

