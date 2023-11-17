cmake_minimum_required(VERSION 3.16.0)

project(dsm_tests)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the C++ flags
string(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -g")

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../)

include(FetchContent)
# Get doctest
FetchContent_Declare(doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG 7b9885133108ae301ddd16e2651320f54cafeba7
)
FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
  FetchContent_Populate(doctest)
endif()

# add as executable all cpp files into '.' folder
file(GLOB SOURCES "*.cpp")

# Define the executable
add_executable(dsm_tests.out ${SOURCES})
target_include_directories(dsm_tests.out PRIVATE ../src/dsm/headers/ ../src/dsm/utility/TypeTraits/)
target_include_directories(dsm_tests.out SYSTEM PRIVATE ${doctest_SOURCE_DIR}/doctest)
