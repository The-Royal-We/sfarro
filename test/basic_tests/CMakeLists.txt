cmake_minimum_required(VERSION 2.8)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

##Find FUSE
#FIND_PACKAGE(FUSE)

set(SOURCE_FILES
        basic_check.cpp
        test_timer.h
        test_timer.cpp
        test_remount.h
        test_remount.cpp
        )

add_executable(runUnitTests ${SOURCE_FILES})

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests sfarro)

