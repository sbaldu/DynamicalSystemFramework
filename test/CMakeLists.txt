cmake_minimum_required(VERSION 3.16.0)

project(Test VERSION 1.0.0 LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add subdirectories
add_subdirectory(./Node)
add_subdirectory(./SparseMatrix)
add_subdirectory(./Street)
