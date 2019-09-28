
cmake_minimum_required(VERSION 2.8.9)
project (Default)
SET(CMAKE_CXX_FLAGS "-g -std=c++17 -Wall -Wextra")
include_directories(include external/Catch2/single_include )
# For the shared library:
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp" )
file(GLOB TEST_SOURCES "tests/test.cpp")
file(GLOB TEST_SOURCES "benchmark/benchmark.cpp")

add_executable(Default ${SOURCES})
add_executable(Tests ${TEST_SOURCES})
add_executable(Benchmarking ${TEST_SOURCES})
