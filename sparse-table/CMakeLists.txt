# Require minimally VERSION 3.21
cmake_minimum_required(VERSION 3.21)

project(sparse-table)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(BINARY sparse_table)

# Glob sources
file(GLOB sources include/*.hpp src/*.cc)

# Make library target
add_library(${BINARY}_lib SHARED ${sources})

target_include_directories(${BINARY}_lib PUBLIC
	${PROJECT_SOURCE_DIR}/include
)
target_compile_options(${BINARY}_lib PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${BINARY}_lib PRIVATE cxx_std_20)

# Set include directories name
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)