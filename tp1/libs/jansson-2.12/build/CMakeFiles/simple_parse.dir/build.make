# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build

# Include any dependencies generated for this target.
include CMakeFiles/simple_parse.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/simple_parse.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/simple_parse.dir/flags.make

CMakeFiles/simple_parse.dir/examples/simple_parse.c.o: CMakeFiles/simple_parse.dir/flags.make
CMakeFiles/simple_parse.dir/examples/simple_parse.c.o: ../examples/simple_parse.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/simple_parse.dir/examples/simple_parse.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/simple_parse.dir/examples/simple_parse.c.o   -c /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/examples/simple_parse.c

CMakeFiles/simple_parse.dir/examples/simple_parse.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple_parse.dir/examples/simple_parse.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/examples/simple_parse.c > CMakeFiles/simple_parse.dir/examples/simple_parse.c.i

CMakeFiles/simple_parse.dir/examples/simple_parse.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple_parse.dir/examples/simple_parse.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/examples/simple_parse.c -o CMakeFiles/simple_parse.dir/examples/simple_parse.c.s

CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.requires:

.PHONY : CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.requires

CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.provides: CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.requires
	$(MAKE) -f CMakeFiles/simple_parse.dir/build.make CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.provides.build
.PHONY : CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.provides

CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.provides.build: CMakeFiles/simple_parse.dir/examples/simple_parse.c.o


# Object files for target simple_parse
simple_parse_OBJECTS = \
"CMakeFiles/simple_parse.dir/examples/simple_parse.c.o"

# External object files for target simple_parse
simple_parse_EXTERNAL_OBJECTS =

bin/simple_parse: CMakeFiles/simple_parse.dir/examples/simple_parse.c.o
bin/simple_parse: CMakeFiles/simple_parse.dir/build.make
bin/simple_parse: lib/libjansson.a
bin/simple_parse: CMakeFiles/simple_parse.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/simple_parse"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_parse.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/simple_parse.dir/build: bin/simple_parse

.PHONY : CMakeFiles/simple_parse.dir/build

CMakeFiles/simple_parse.dir/requires: CMakeFiles/simple_parse.dir/examples/simple_parse.c.o.requires

.PHONY : CMakeFiles/simple_parse.dir/requires

CMakeFiles/simple_parse.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/simple_parse.dir/cmake_clean.cmake
.PHONY : CMakeFiles/simple_parse.dir/clean

CMakeFiles/simple_parse.dir/depend:
	cd /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12 /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12 /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build /home/ju/Projects/CLionProjects/INF3172/tp1/libs/jansson-2.12/build/CMakeFiles/simple_parse.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/simple_parse.dir/depend

