cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(../src/)

add_executable(runTimerTests test_timer.cc ../src/timer.h)

target_link_libraries(runTimerTests gtest gtest_main)
target_link_libraries(runTimerTests ../src/timer.h)