//
// Created by se415011 on 10/14/15.
//

#include <stdlib.h>
#include <jmorecfg.h>
#include "filesystem_handler.h"
#include "vfs.h"
#include "constants.h"


int change_to_read_only(char const *argv[]) {

    int res = 0;
    const char *path;

    path = argv[0];

    if(fs_not_read(path))
        res = mount(path, path, MS_RDONLY,"","");

    if (res == errno)
        return -errno;

    return res;
}

boolean fs_not_read(const char *path) {

    time_t rawtime;
    time_t fs_time;
    struct tm * current_timeinfo;
    struct tm * fs_timeinfo;
    int res;

    time(&rawtime);

    current_timeinfo = localtime(&rawtime);


    fs_time = get_mtime(path);
    return current_timeinfo->tm_sec - fs_timeinfo->tm_sec > 5000;
}

time_t get_mtime(const char *path) {

    struct stat statbuf;

    if(stat(path, &statbuf) == errno ){
        perror(path);
        exit(1);
    }
    return statbuf.st_mtime;
}