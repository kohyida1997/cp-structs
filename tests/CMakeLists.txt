# Require minimally VERSION 3.21
cmake_minimum_required(VERSION 3.21)

project(tests)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(SPARSE_TB_BIN sparse_table)

# Glob sources
file(GLOB sources include/*.hpp src/*.cc)

# Make exec target
add_executable(${SPARSE_TB_BIN}_test1 ${sources})
target_compile_options(${SPARSE_TB_BIN}_test1 PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${SPARSE_TB_BIN}_test1 PRIVATE cxx_std_20)
target_link_libraries(${SPARSE_TB_BIN}_test1 PRIVATE 
    sparse_table_lib
)
target_include_directories(${SPARSE_TB_BIN}_test1 PUBLIC
	${sparse_table_INCLUDE_DIRS}
)
