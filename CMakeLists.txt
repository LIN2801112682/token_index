cmake_minimum_required (VERSION 3.0.0)

project (token_index VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

aux_source_directory(src/token_index TOKEN_INDEX)
aux_source_directory(src/bm BM)

add_executable (test_field_manager tests/test_field_manager.cpp ${TOKEN_INDEX} ${BM})
add_executable (filter tests/filter.cpp)
add_executable (test_bm tests/test_bm.cpp ${BM})