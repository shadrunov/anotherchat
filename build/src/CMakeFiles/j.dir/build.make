# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/alex/Documents/asio-example-main

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alex/Documents/asio-example-main/build

# Include any dependencies generated for this target.
include src/CMakeFiles/j.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/j.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/j.dir/flags.make

src/CMakeFiles/j.dir/j.cpp.o: src/CMakeFiles/j.dir/flags.make
src/CMakeFiles/j.dir/j.cpp.o: ../src/j.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alex/Documents/asio-example-main/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/j.dir/j.cpp.o"
	cd /home/alex/Documents/asio-example-main/build/src && /usr/bin/ccache /bin/x86_64-linux-gnu-g++-10  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/j.dir/j.cpp.o -c /home/alex/Documents/asio-example-main/src/j.cpp

src/CMakeFiles/j.dir/j.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/j.dir/j.cpp.i"
	cd /home/alex/Documents/asio-example-main/build/src && /bin/x86_64-linux-gnu-g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alex/Documents/asio-example-main/src/j.cpp > CMakeFiles/j.dir/j.cpp.i

src/CMakeFiles/j.dir/j.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/j.dir/j.cpp.s"
	cd /home/alex/Documents/asio-example-main/build/src && /bin/x86_64-linux-gnu-g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alex/Documents/asio-example-main/src/j.cpp -o CMakeFiles/j.dir/j.cpp.s

# Object files for target j
j_OBJECTS = \
"CMakeFiles/j.dir/j.cpp.o"

# External object files for target j
j_EXTERNAL_OBJECTS =

bin/j: src/CMakeFiles/j.dir/j.cpp.o
bin/j: src/CMakeFiles/j.dir/build.make
bin/j: lib/libeasy-encryption.a
bin/j: lib/libdoctest.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_system.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_coroutine.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_context.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_thread.a
bin/j: /usr/lib/x86_64-linux-gnu/libboost_atomic.a
bin/j: /home/alex/.conan/data/fmt/7.1.2/_/_/package/a25d6c83542b56b72fdaa05a85db5d46f5f0f71c/lib/libfmtd.a
bin/j: src/CMakeFiles/j.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alex/Documents/asio-example-main/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/j"
	cd /home/alex/Documents/asio-example-main/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/j.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/j.dir/build: bin/j

.PHONY : src/CMakeFiles/j.dir/build

src/CMakeFiles/j.dir/clean:
	cd /home/alex/Documents/asio-example-main/build/src && $(CMAKE_COMMAND) -P CMakeFiles/j.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/j.dir/clean

src/CMakeFiles/j.dir/depend:
	cd /home/alex/Documents/asio-example-main/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alex/Documents/asio-example-main /home/alex/Documents/asio-example-main/src /home/alex/Documents/asio-example-main/build /home/alex/Documents/asio-example-main/build/src /home/alex/Documents/asio-example-main/build/src/CMakeFiles/j.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/j.dir/depend
