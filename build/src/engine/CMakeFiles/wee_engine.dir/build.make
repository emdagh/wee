# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/emda/git/wee

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/emda/git/wee/build

# Include any dependencies generated for this target.
include src/engine/CMakeFiles/wee_engine.dir/depend.make

# Include the progress variables for this target.
include src/engine/CMakeFiles/wee_engine.dir/progress.make

# Include the compile flags for this target's objects.
include src/engine/CMakeFiles/wee_engine.dir/flags.make

src/engine/CMakeFiles/wee_engine.dir/assets.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/assets.cpp.o: ../src/engine/assets.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/assets.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/assets.cpp.o -c /home/emda/git/wee/src/engine/assets.cpp

src/engine/CMakeFiles/wee_engine.dir/assets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/assets.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/assets.cpp > CMakeFiles/wee_engine.dir/assets.cpp.i

src/engine/CMakeFiles/wee_engine.dir/assets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/assets.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/assets.cpp -o CMakeFiles/wee_engine.dir/assets.cpp.s

src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.o: ../src/engine/b2Adapters.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/b2Adapters.cpp.o -c /home/emda/git/wee/src/engine/b2Adapters.cpp

src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/b2Adapters.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/b2Adapters.cpp > CMakeFiles/wee_engine.dir/b2Adapters.cpp.i

src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/b2Adapters.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/b2Adapters.cpp -o CMakeFiles/wee_engine.dir/b2Adapters.cpp.s

src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o: ../src/engine/b2ContactListenerImpl.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o -c /home/emda/git/wee/src/engine/b2ContactListenerImpl.cpp

src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/b2ContactListenerImpl.cpp > CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.i

src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/b2ContactListenerImpl.cpp -o CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.s

src/engine/CMakeFiles/wee_engine.dir/easing.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/easing.cpp.o: ../src/engine/easing.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/easing.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/easing.cpp.o -c /home/emda/git/wee/src/engine/easing.cpp

src/engine/CMakeFiles/wee_engine.dir/easing.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/easing.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/easing.cpp > CMakeFiles/wee_engine.dir/easing.cpp.i

src/engine/CMakeFiles/wee_engine.dir/easing.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/easing.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/easing.cpp -o CMakeFiles/wee_engine.dir/easing.cpp.s

src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.o: ../src/engine/ecs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/ecs.cpp.o -c /home/emda/git/wee/src/engine/ecs.cpp

src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/ecs.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/ecs.cpp > CMakeFiles/wee_engine.dir/ecs.cpp.i

src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/ecs.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/ecs.cpp -o CMakeFiles/wee_engine.dir/ecs.cpp.s

src/engine/CMakeFiles/wee_engine.dir/packer.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/packer.cpp.o: ../src/engine/packer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/packer.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/packer.cpp.o -c /home/emda/git/wee/src/engine/packer.cpp

src/engine/CMakeFiles/wee_engine.dir/packer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/packer.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/packer.cpp > CMakeFiles/wee_engine.dir/packer.cpp.i

src/engine/CMakeFiles/wee_engine.dir/packer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/packer.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/packer.cpp -o CMakeFiles/wee_engine.dir/packer.cpp.s

src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.o: ../src/engine/sprite_font.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/sprite_font.cpp.o -c /home/emda/git/wee/src/engine/sprite_font.cpp

src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/sprite_font.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/sprite_font.cpp > CMakeFiles/wee_engine.dir/sprite_font.cpp.i

src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/sprite_font.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/sprite_font.cpp -o CMakeFiles/wee_engine.dir/sprite_font.cpp.s

src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o: ../src/engine/sprite_sheet.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o -c /home/emda/git/wee/src/engine/sprite_sheet.cpp

src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/sprite_sheet.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/sprite_sheet.cpp > CMakeFiles/wee_engine.dir/sprite_sheet.cpp.i

src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/sprite_sheet.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/sprite_sheet.cpp -o CMakeFiles/wee_engine.dir/sprite_sheet.cpp.s

src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.o: src/engine/CMakeFiles/wee_engine.dir/flags.make
src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.o: ../src/engine/viewport.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.o"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wee_engine.dir/viewport.cpp.o -c /home/emda/git/wee/src/engine/viewport.cpp

src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wee_engine.dir/viewport.cpp.i"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emda/git/wee/src/engine/viewport.cpp > CMakeFiles/wee_engine.dir/viewport.cpp.i

src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wee_engine.dir/viewport.cpp.s"
	cd /home/emda/git/wee/build/src/engine && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emda/git/wee/src/engine/viewport.cpp -o CMakeFiles/wee_engine.dir/viewport.cpp.s

# Object files for target wee_engine
wee_engine_OBJECTS = \
"CMakeFiles/wee_engine.dir/assets.cpp.o" \
"CMakeFiles/wee_engine.dir/b2Adapters.cpp.o" \
"CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o" \
"CMakeFiles/wee_engine.dir/easing.cpp.o" \
"CMakeFiles/wee_engine.dir/ecs.cpp.o" \
"CMakeFiles/wee_engine.dir/packer.cpp.o" \
"CMakeFiles/wee_engine.dir/sprite_font.cpp.o" \
"CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o" \
"CMakeFiles/wee_engine.dir/viewport.cpp.o"

# External object files for target wee_engine
wee_engine_EXTERNAL_OBJECTS =

lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/assets.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/b2Adapters.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/b2ContactListenerImpl.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/easing.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/ecs.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/packer.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/sprite_font.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/sprite_sheet.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/viewport.cpp.o
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/build.make
lib/Debug/libwee_engine.so: src/engine/CMakeFiles/wee_engine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/emda/git/wee/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX shared library ../../lib/Debug/libwee_engine.so"
	cd /home/emda/git/wee/build/src/engine && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wee_engine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/engine/CMakeFiles/wee_engine.dir/build: lib/Debug/libwee_engine.so

.PHONY : src/engine/CMakeFiles/wee_engine.dir/build

src/engine/CMakeFiles/wee_engine.dir/clean:
	cd /home/emda/git/wee/build/src/engine && $(CMAKE_COMMAND) -P CMakeFiles/wee_engine.dir/cmake_clean.cmake
.PHONY : src/engine/CMakeFiles/wee_engine.dir/clean

src/engine/CMakeFiles/wee_engine.dir/depend:
	cd /home/emda/git/wee/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/emda/git/wee /home/emda/git/wee/src/engine /home/emda/git/wee/build /home/emda/git/wee/build/src/engine /home/emda/git/wee/build/src/engine/CMakeFiles/wee_engine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/engine/CMakeFiles/wee_engine.dir/depend

