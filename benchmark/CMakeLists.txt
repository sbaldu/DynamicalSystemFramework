cmake_minimum_required(VERSION 3.16.0)

project(Benchmark VERSION 1.0.1 LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../extern/benchmark/)

include(FetchContent)
# Get spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0
)
FetchContent_MakeAvailable(spdlog)

# add subdirectories
add_subdirectory(Graph)
add_subdirectory(Street)
add_subdirectory(Dynamics)
