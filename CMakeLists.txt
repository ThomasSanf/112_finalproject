cmake_minimum_required(VERSION 3.27)
project(untitled C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/jetbrainslibs/SDL2/x86_64-w64-mingw32")
set(SDL2_TTF_PATH "C:/jetbrainslibs/SDL2_ttf/x86_64-w64-mingw32")
# Add the following lines to your CMakeLists.txt file

# Copy the TTF font file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/Minecraft.ttf ${CMAKE_BINARY_DIR}Minecraft.ttf COPYONLY)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

add_executable(untitled main.c)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES})
