#include "monitor.h"

// TODO: Add in error handling
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
            fprintf(stderr, "Remounting filesystem to READ_ONLY!");
            set_vfs_to_read_only();
            return 0;
        }
    }
    return 0;
}

void
init_sfarro_monitor ()
{
    long initial_time = LONG_MAX;
    set_last_time_written (&initial_time);
    pthread_t tid;
    pthread_create (&tid, NULL, &threadproc, NULL);
    return;
}
