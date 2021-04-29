cmake_minimum_required (VERSION 3.0.0)

project (token_index VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(INDEX_MANAGER_V1 src/token_index/common.cpp src/token_index/index_manager_v1.cpp)
set(INDEX_MANAGER_V2 src/token_index/common.cpp src/token_index/index_manager_v2.cpp)
set(BM src/bm/bm.cpp)

add_executable (test_index_manager_v1 tests/test_index_manager_v1.cpp ${INDEX_MANAGER_V1} ${BM})
add_executable (test_index_manager_v2 tests/test_index_manager_v2.cpp ${INDEX_MANAGER_V2} ${BM})
add_executable (filter tests/filter.cpp)
add_executable (test_bm tests/test_bm.cpp ${BM})