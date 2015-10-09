cmake_minimum_required(VERSION 2.8)
project(sfarro)

#Finding FUSE
FIND_PACKAGE(FUSE)

MESSAGE("HELLO WORLD")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})


#Won't build unless this is specified..
add_definitions(-D_FILE_OFFSET_BITS=64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/sfarro.c
    src/vfs.c
    README.md)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libfuse2")

add_executable(sfarro ${SOURCE_FILES})