/**
 * We run a timed function call that periodically checks if the vfs can be remounted safely
 * We then call remount(2) on the vfs.
 */

#include "remount.h"

bool periodic_remount_check() {
    return ready_to_remount();
}

void *threadproc() {
    int done = 1;
    while (1) {
        sleep(PERIODIC_TIME_DELAY);
        if (periodic_remount_check() == 1) {
            if (remount() < 0)
//              Something happened..
                return 0;
        }
    }
    return 0;
}

void init_sfarro_monitor() {
    pthread_t tid;
    pthread_create(&tid, NULL, &threadproc, NULL);
    return;
}

int remount() {
    int res;
    res = mount(VFS_DATA->rootdir, VFS_DATA->rootdir, MS_REMOUNT | O_RDONLY, NULL, NULL);
    return 0;
}
