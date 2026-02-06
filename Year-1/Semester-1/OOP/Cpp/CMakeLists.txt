cmake_minimum_required(VERSION 3.14)
project(University_Repository)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE CPP_FILES "OOP/**/*.cpp")
file(GLOB_RECURSE C_FILES "OOP/**/*.c")

foreach(src_file ${CPP_FILES})
    # Get filename without path and extension for target name
    get_filename_component(fname ${src_file} NAME_WE)
    # Get relative path for unique target name
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/OOP" "${src_file}")
    string(REPLACE "/" "_" tname "${rel_path}")
    string(REPLACE "." "_" tname "${tname}")
    add_executable(${tname} ${src_file})
endforeach()

foreach(src_file ${C_FILES})
    get_filename_component(fname ${src_file} NAME_WE)
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/OOP" "${src_file}")
    string(REPLACE "/" "_" tname "${rel_path}")
    string(REPLACE "." "_" tname "${tname}")
    add_executable(${tname} ${src_file})
endforeach()