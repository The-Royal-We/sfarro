//
// Created by se415011 on 11/11/15.
//
#include "monitor.h"

bool periodic_remount_check() {
    return ready_to_remount();
}

// TODO: Add in error handling
void *threadproc() {
    char *vfs_root_directory = VFS_DATA->rootdir;
    while (1) {
        sleep(PERIODIC_TIME_DELAY);
        if (periodic_remount_check() == 1) {
            if (remount(vfs_root_directory) != 0) {
//                Something fkd up
                return 0;
            }
        }
    }
    return 0;
}

void init_sfarro_monitor() {
    long initial_time = LONG_MAX;
    set_last_time_written(&initial_time);
    pthread_t tid;
    pthread_create(&tid, NULL, &threadproc, NULL);
    return;
}