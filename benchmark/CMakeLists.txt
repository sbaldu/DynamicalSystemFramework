cmake_minimum_required(VERSION 3.16.0)

project(benchmark VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../)

include(FetchContent)
# Get bench
FetchContent_Declare(bench
  GIT_REPOSITORY https://github.com/sbaldu/benchmark
  GIT_TAG 74faa8760ba92928e9cd9d8ee6a52da370dcd4aa
)
FetchContent_GetProperties(bench)
if(NOT bench_POPULATED)
  FetchContent_Populate(bench)
endif()

# add as executable all cpp files into '.' folder
file(GLOB SOURCES "*.cpp")

# Define the executable
add_executable(benchmark.out ${SOURCES})
target_include_directories(benchmark.out PRIVATE ../src/dsm/headers/ ../src/dsm/utility/TypeTraits/)
target_include_directories(benchmark.out SYSTEM PRIVATE ${bench_SOURCE_DIR}/)
