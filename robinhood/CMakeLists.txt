# Require minimally VERSION 3.21
cmake_minimum_required(VERSION 3.21)

project(robinhood)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(BINARY robinhood)

# Glob sources
file(GLOB_RECURSE sources include/*.hpp)

# Make library target
add_library(${BINARY}_lib SHARED ${sources})
set_target_properties(${BINARY}_lib PROPERTIES LINKER_LANGUAGE CXX)

# Glob include dirs
file(GLOB interface_include_dirs include/*)
target_include_directories(${BINARY}_lib INTERFACE
	${interface_include_dirs}
)

target_compile_options(${BINARY}_lib PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${BINARY}_lib PRIVATE cxx_std_20)

# Set include directories name
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
