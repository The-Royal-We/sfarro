cmake_minimum_required(VERSION 2.8)
project(sfarro)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Find FUSE
FIND_PACKAGE(FUSE)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lulockmgr")

set(SOURCE_FILES
    src/sfarro.c
    src/vfs.c
    src/vfs.h
    src/constants.h
    src/log.h
    src/log.c
    src/params.h
)
#        src/filesystem_handler.c
#        src/filesystem_handler.h)

add_executable(sfarro ${SOURCE_FILES} src/vfs.h )#src/filesystem_handler.c src/filesystem_handler.h)

target_include_directories(sfarro PRIVATE ${FUSE_INCLUDE_DIR})
target_link_libraries(sfarro ${FUSE_LIBRARIES})