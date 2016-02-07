cmake_minimum_required(VERSION 3.2.3)
project(src)
#
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
#
##Find FUSE
#FIND_PACKAGE(FUSE)
#
##Find Thread package
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#
#set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")
#
#include_directories(src)
#
#add_subdirectory(src)
#
#set(SOURCE_FILES main.c)
#
#add_executable(main ${SOURCE_FILES})
#
#target_include_directories(main PRIVATE ${FUSE_INCLUDE_DIR})
#target_link_libraries(main src ${FUSE_LIBRARIES} Threads::Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(src COMMAND make -C ${sfarro_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})
