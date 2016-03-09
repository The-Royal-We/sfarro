#include "monitor.h"

// TODO: Add in error handling
struct vfs_state *vfs_data;
void *
threadproc ()
{
    while (1)
    {
        fprintf(stderr, "Checking filesystem\n");
        sleep (PERIODIC_TIME_DELAY);
        if (ready_to_remount())
        {
            fprintf(stderr, "Periodic mount check passed\n");
            fprintf(stderr, "Remounting mountpoint to READ_ONLY!");
//            remount(vfs_data->mountdir);
            return 0;
        }
    }
    return 0;
}

void
init_sfarro_monitor (struct vfs_state *vfs_d)
{
    long initial_time = LONG_MAX;
    vfs_data = vfs_d;
    set_last_time_written (&initial_time);
    pthread_t tid;
    pthread_create (&tid, NULL, &threadproc, NULL);
    return;
}
