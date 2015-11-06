cmake_minimum_required(VERSION 3.2.3)
project(sfarro)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

#Find FUSE
FIND_PACKAGE(FUSE)

#Find Thread package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Set up GTest framework
add_subdirectory(test/lib/gtest-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#Unit tests
add_executable(runUnitTests test/test_timer.cc)
target_link_libraries(runUnitTests gtest gtest_main)
add_test(runUnitTests runUnitTests)


set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")

set(SOURCE_FILES
        src/sfarro.c
        src/vfs.c
        src/vfs.h
        src/constants.h
        src/log.h
        src/log.c
        src/params.h
        src/timer.h
        src/timer.c
        src/remount.h
        src/remount.c
)

add_executable(sfarro ${SOURCE_FILES})

target_include_directories(sfarro PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(sfarro ${FUSE_LIBRARIES} Threads::Threads)