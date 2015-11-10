cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runUnitTests basic_check.cpp test_timer.cpp)

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests sfarro)