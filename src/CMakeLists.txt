cmake_minimum_required(VERSION 2.8)

project(src)

set(HEADER_FILES
        constants.h
        remount.h
        timer.h
        vfs.h
        monitor.h
        params.h
        )

set(SOURCE_FILES
        remount.c
        timer.c
        monitor.c
        vfs.c
        )

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})