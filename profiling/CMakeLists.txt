cmake_minimum_required(VERSION 3.16.0)

project(prof VERSION 1.0.1)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the C++ flags
string(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -g -pg")

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../)

# add as executable all cpp files into '.' folder
file(GLOB SOURCES "*.cpp")

# Define the executable
add_executable(dsm_prof.out ${SOURCES})
target_include_directories(dsm_prof.out PRIVATE ../src/)
