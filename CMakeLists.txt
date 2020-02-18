
cmake_minimum_required(VERSION 2.8.9)
project (Default)
SET(CMAKE_CXX_FLAGS "-g -std=c++2a  -Wall -Wextra -Wswitch-enum")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(DOWNLOAD https://raw.githubusercontent.com/tcbrindle/NanoRange/master/single_include/nanorange.hpp external/NanoRange/nanorange.hpp)
include_directories(include external/Catch2/single_include external/picobench/include/picobench external/NanoRange/include/)
# For the shared library:
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp" )
file(GLOB TEST_SOURCES "tests/*.cpp")
file(GLOB BENCH_SOURCES "benchmark/benchmark.cpp")

#set(CMAKE_CXX_CPPCHECK "cppcheck")
add_executable(Default ${SOURCES})
add_executable(Tests ${TEST_SOURCES})
add_executable(Benchmarking ${BENCH_SOURCES})
