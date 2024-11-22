cmake_minimum_required(VERSION 3.10)

# Project name
project(IFJ C)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Enable all warnings and debugging symbols
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Find all source files
file(GLOB SRCS "*.c")

# Add executable target
add_executable(program ${SRCS})

# Custom target for cleaning build files
add_custom_target(clean_build_files
    COMMAND ${CMAKE_COMMAND} -E remove program ${CMAKE_BINARY_DIR}/*.o
    COMMENT "Cleaning up build files"
)