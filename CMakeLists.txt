cmake_minimum_required (VERSION 3.0.0)

project (TokenIndex VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(SRC src/token_index/token_manager.cpp src/token_index/BM.cpp)

add_executable (test tests/test.cpp ${SRC})
add_executable (filter tests/filter.cpp)