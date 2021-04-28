cmake_minimum_required (VERSION 3.0.0)

project (TokenIndex VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(TOKEN_INDEX src/token_index/common.cpp src/token_index/index_manager.cpp)
set(BM src/bm/bm.cpp)

add_executable (test tests/test.cpp ${TOKEN_INDEX} ${BM})
add_executable (filter tests/filter.cpp)
add_executable (test_bm tests/test_bm.cpp ${BM})