cmake_minimum_required(VERSION 3.16.0)

project(dms VERSION 1.0.0 LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(test)
# copy test/data folder in order to make tests use it
file(COPY ${PROJECT_SOURCE_DIR}/test/data DESTINATION ${PROJECT_BINARY_DIR}/test)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION /usr/include)
