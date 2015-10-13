//
// Created by se415011 on 10/13/15.
//

#ifndef SFARRO_PARAMS_H
#define SFARRO_PARAMS_H

#define FUSE_USE_VERSION 26

#define _XOPEN_SOURCE 500

#include <limits.h>
#include <stdio.h>

struct vfs_state {
    FILE *logfile;
    char *rootdir;
};
#define VFS_DATA ((struct vfs_state *) fuse_get_context()->private_data)

#endif //SFARRO_PARAMS_H
