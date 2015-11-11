/**
 * We run a timed function call that periodically checks if the vfs can be remounted safely
 * We then call system library function: remount(2) on the vfs.
 */

#include "remount.h"

int remount(char *remount_directory) {
    int res;
    res = mount(remount_directory, remount_directory, MS_REMOUNT | O_RDONLY, NULL, NULL);
    printf("\n **** Remount occured at %s ***** \n", remount_directory);
    return 0;
}
