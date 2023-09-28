# Require minimally VERSION 3.21
cmake_minimum_required(VERSION 3.21)

project(tests)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(SPARSE_TB_BIN sparse_table)

# Glob sources
file(GLOB sources 
	include/*.hpp 
	include/*.cpp
	sparse_table/*.cc
	sparse_table/*.hpp)

# Common include and compile options
add_compile_options(-Wall -Wextra -pedantic)

# TestSum - Target
set(SPARSE_TB_TESTSUM_BIN ${SPARSE_TB_BIN}_TestSum)
add_executable(${SPARSE_TB_TESTSUM_BIN}
	${PROJECT_SOURCE_DIR}/sparse_table/SparseTb_TestSum.cc
)
target_include_directories(${SPARSE_TB_TESTSUM_BIN} PUBLIC
	${sparse_table_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/include
)
target_compile_options(${SPARSE_TB_TESTSUM_BIN} PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${SPARSE_TB_TESTSUM_BIN} PRIVATE cxx_std_20)
target_link_libraries(${SPARSE_TB_TESTSUM_BIN} PRIVATE 
    sparse_table_lib
)

# TestMinMax - Target
set(SPARSE_TB_TESTMINMAX_BIN ${SPARSE_TB_BIN}_TestMinMax)
add_executable(${SPARSE_TB_TESTMINMAX_BIN} 
	${PROJECT_SOURCE_DIR}/sparse_table/SparseTb_TestMinMax.cc
)
target_include_directories(${SPARSE_TB_TESTMINMAX_BIN} PUBLIC
	${sparse_table_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${SPARSE_TB_TESTMINMAX_BIN} PRIVATE 
    sparse_table_lib
)

# Robinhood_Set_TestFixedSize - Target
add_executable(robinhood_set_TestFixedSize 
	${PROJECT_SOURCE_DIR}/robinhood/Robinhood_Set_TextFixedSize.cc)
target_include_directories(robinhood_set_TestFixedSize PUBLIC
	${robinhood_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/include)
target_link_libraries(robinhood_set_TestFixedSize PRIVATE
	robinhood_lib)
