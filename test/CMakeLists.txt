cmake_minimum_required(VERSION 2.8)

project(test)

set(CMAKE_CXX_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")

add_subdirectory(lib/gtest-1.7.0)
add_subdirectory(basic_tests)