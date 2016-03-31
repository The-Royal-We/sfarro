/** @file
* Params.h contains definitons for our vfs_state struct
* and a macro definition to pull the struct from the FUSE
* context
*/

#ifndef SFARRO_PARAMS_H
#define SFARRO_PARAMS_H

#include <limits.h>
#include <stdio.h>
/** @struct vfs_state
*	@brief This structure contains the paths to our running root
			directory and the mount directory
*	@var *rootdir
*	This member contains the file path to SFARRO's working root directory.
*	@var mountdir
*	This member contains the file path to the systems mount device.

*/
struct vfs_state {
    char *rootdir;
    char *mountdir;
};

#define VFS_DATA ((struct vfs_state *) fuse_get_context()->private_data)

#endif //SFARRO_PARAMS_H
