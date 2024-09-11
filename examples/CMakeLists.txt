cmake_minimum_required(VERSION 3.16.0)

project(dsm_examples VERSION 1.3.10 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the C++ flags
string(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -g")

# Set the folder for the executable
set(EXECUTABLE_OUTPUT_PATH ../)

# add as executable all cpp files into '.' folder
file(GLOB SOURCES "*.cpp")

# Loop through each source file and create an executable
foreach(SOURCE ${SOURCES})
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME}.out ${SOURCE})
    target_include_directories(${EXE_NAME}.out PRIVATE ../src/dsm/headers/ ../src/dsm/utility/TypeTraits/)
endforeach()
