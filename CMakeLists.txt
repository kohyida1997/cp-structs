# Require minimally VERSION 3.21
cmake_minimum_required(VERSION 3.21)

project(cp-structs)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_VERBOSE_MAKEFILE ON) 

enable_testing()

include_directories(
  sparse-table
  tests
)

add_subdirectory(sparse-table)
add_subdirectory(tests)
