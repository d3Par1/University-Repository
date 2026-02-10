cmake_minimum_required(VERSION 3.10)
project(Task1_3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Static library: process1
add_library(process1_static STATIC process1.c)
# Shared library: process1
add_library(process1_shared SHARED process1.c)
set_target_properties(process1_shared PROPERTIES OUTPUT_NAME process1)

# Static library: process
add_library(process_static STATIC process.c)
# Shared library: process
add_library(process_shared SHARED process.c)
set_target_properties(process_shared PROPERTIES OUTPUT_NAME process)

# Main executable (static linking)
add_executable(my_program main.c input.c output.c)
target_link_libraries(my_program process1_static process_static)

# Main executable (shared linking)
add_executable(my_program_shared main.c input.c output.c)
target_link_libraries(my_program_shared process1_shared process_shared)
