#ifndef SFARRO_PARAMS_H
#define SFARRO_PARAMS_H

#include <limits.h>
#include <stdio.h>

struct vfs_state {
    char *rootdir;
    char *mountdir;
};

#define VFS_DATA ((struct vfs_state *) fuse_get_context()->private_data)

#endif //SFARRO_PARAMS_H
