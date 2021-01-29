cmake_minimum_required(VERSION 3.9)

# Library locations (Change these to the appropriate path on your machine)
set(INCLUDE_DIR C:/DevelopmentLibraries/include/)
set(LIB_DIR C:/DevelopmentLibraries/lib/x64)

# Name the project
project(VNES_project)

# Find all source files
file(GLOB_RECURSE VNES_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

# Create a static library
add_executable(VNES ${VNES_SRCS})

# Tell VNES where to look for includes and libs
target_include_directories(VNES PUBLIC ${INCLUDE_DIR})
target_link_directories(VNES PUBLIC ${LIB_DIR})

# Link Libraries
target_link_libraries(VNES V6502)
target_link_libraries(VNES SDL2)
target_link_libraries(VNES SDL2main)