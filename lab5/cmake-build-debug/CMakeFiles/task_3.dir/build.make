# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /snap/clion/169/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/169/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniel/CLionProjects/parallelLabs/lab5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/task_3.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/task_3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/task_3.dir/flags.make

CMakeFiles/task_3.dir/task_3.cpp.o: CMakeFiles/task_3.dir/flags.make
CMakeFiles/task_3.dir/task_3.cpp.o: ../task_3.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/task_3.dir/task_3.cpp.o"
	/home/daniel/opt/usr/local/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/task_3.dir/task_3.cpp.o -c /home/daniel/CLionProjects/parallelLabs/lab5/task_3.cpp

CMakeFiles/task_3.dir/task_3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/task_3.dir/task_3.cpp.i"
	/home/daniel/opt/usr/local/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniel/CLionProjects/parallelLabs/lab5/task_3.cpp > CMakeFiles/task_3.dir/task_3.cpp.i

CMakeFiles/task_3.dir/task_3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/task_3.dir/task_3.cpp.s"
	/home/daniel/opt/usr/local/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniel/CLionProjects/parallelLabs/lab5/task_3.cpp -o CMakeFiles/task_3.dir/task_3.cpp.s

# Object files for target task_3
task_3_OBJECTS = \
"CMakeFiles/task_3.dir/task_3.cpp.o"

# External object files for target task_3
task_3_EXTERNAL_OBJECTS =

task_3: CMakeFiles/task_3.dir/task_3.cpp.o
task_3: CMakeFiles/task_3.dir/build.make
task_3: CMakeFiles/task_3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable task_3"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/task_3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/task_3.dir/build: task_3
.PHONY : CMakeFiles/task_3.dir/build

CMakeFiles/task_3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/task_3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/task_3.dir/clean

CMakeFiles/task_3.dir/depend:
	cd /home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daniel/CLionProjects/parallelLabs/lab5 /home/daniel/CLionProjects/parallelLabs/lab5 /home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug /home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug /home/daniel/CLionProjects/parallelLabs/lab5/cmake-build-debug/CMakeFiles/task_3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/task_3.dir/depend

