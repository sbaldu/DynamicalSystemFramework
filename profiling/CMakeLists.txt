cmake_minimum_required(VERSION 3.16.0)

project(prof VERSION 1.0.1)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../)

# Define the executable
add_executable(prof.out main.cpp)
target_include_directories(prof.out PRIVATE ../src/)
target_compile_options(prof.out PRIVATE -pg -Os)
target_link_options(prof.out PRIVATE -pg)

add_executable(mem.out main.cpp)
target_include_directories(mem.out PRIVATE ../src/)
target_compile_options(mem.out PRIVATE -Os)

add_executable(parse_massif.out parse_massif.cpp)

add_executable(cache.out main.cpp)
target_include_directories(cache.out PRIVATE ../src/)
target_compile_options(cache.out PRIVATE -pg -O2)
