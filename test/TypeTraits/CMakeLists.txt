cmake_minimum_required(VERSION 3.16.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the C++ flags
string(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -g")

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../../)

include_directories(../../src/dmf/utility/TypeTraits/)

# Compile
add_executable(is_numeric.out ./is_numeric.cpp)
add_executable(is_node.out ./is_node.cpp)
add_executable(is_street.out ./is_street.cpp)
