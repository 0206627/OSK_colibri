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
CMAKE_SOURCE_DIR = /home/ricar/Documents/OpenSatKit-master/cfs/cfe

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ricar/Documents/OpenSatKit-master/cfs/build

# Include any dependencies generated for this target.
include tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/depend.make

# Include the progress variables for this target.
include tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/progress.make

# Include the compile flags for this target's objects.
include tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/flags.make

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/flags.make
tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o: /home/ricar/Documents/OpenSatKit-master/cfs/tools/elf2cfetbl/elf2cfetbl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ricar/Documents/OpenSatKit-master/cfs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o"
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o   -c /home/ricar/Documents/OpenSatKit-master/cfs/tools/elf2cfetbl/elf2cfetbl.c

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.i"
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ricar/Documents/OpenSatKit-master/cfs/tools/elf2cfetbl/elf2cfetbl.c > CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.i

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.s"
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ricar/Documents/OpenSatKit-master/cfs/tools/elf2cfetbl/elf2cfetbl.c -o CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.s

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.requires:

.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.requires

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.provides: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.requires
	$(MAKE) -f tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/build.make tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.provides.build
.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.provides

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.provides.build: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o


# Object files for target elf2cfetbl
elf2cfetbl_OBJECTS = \
"CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o"

# External object files for target elf2cfetbl
elf2cfetbl_EXTERNAL_OBJECTS =

tools/elf2cfetbl/elf2cfetbl: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o
tools/elf2cfetbl/elf2cfetbl: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/build.make
tools/elf2cfetbl/elf2cfetbl: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ricar/Documents/OpenSatKit-master/cfs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable elf2cfetbl"
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/elf2cfetbl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/build: tools/elf2cfetbl/elf2cfetbl

.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/build

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/requires: tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/elf2cfetbl.c.o.requires

.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/requires

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/clean:
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl && $(CMAKE_COMMAND) -P CMakeFiles/elf2cfetbl.dir/cmake_clean.cmake
.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/clean

tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/depend:
	cd /home/ricar/Documents/OpenSatKit-master/cfs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ricar/Documents/OpenSatKit-master/cfs/cfe /home/ricar/Documents/OpenSatKit-master/cfs/tools/elf2cfetbl /home/ricar/Documents/OpenSatKit-master/cfs/build /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl /home/ricar/Documents/OpenSatKit-master/cfs/build/tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/elf2cfetbl/CMakeFiles/elf2cfetbl.dir/depend

