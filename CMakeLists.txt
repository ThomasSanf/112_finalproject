cmake_minimum_required(VERSION 3.26)
project(final C)

set(CMAKE_C_STANDARD 11)

# Add your executable
add_executable(final main.c)

# Specify the SDL2 paths
set(SDL2_INCLUDE_DIR "/opt/homebrew/include/SDL2")
set(SDL2_LIBRARY "/opt/homebrew/lib/libSDL2.dylib")

# Include SDL2 directories and link the library
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(final ${SDL2_LIBRARY})
