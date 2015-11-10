cmake_minimum_required(VERSION 2.8)

project(sfarro)

set(HEADER_FILES
        constants.h
        log.h
        remount.h
        timer.h
        vfs.h
        params.h
        )

set(SOURCE_FILES
        log.c
        remount.c
        timer.c
        vfs.c
        )

add_library(sfarro STATIC ${SOURCE_FILES} ${HEADER_FILES})