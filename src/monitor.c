#include "monitor.h"

void init_sfarro_monitor();
char *remount_directory;
bool
periodic_remount_check ()
{
    return ready_to_remount ();
}

// TODO: Add in error handling
void *
threadproc ()
{
    while (1)
    {
        fprintf(stderr, "Checking filesystem\n");
        sleep (PERIODIC_TIME_DELAY);
        if (periodic_remount_check () == 1)
        {
            fprintf(stderr, "periodic mount check passed \n");
            if (remount (remount_directory) != 0)
            {
                return 0;
            }
        }
    }
    return 0;
}

void
init_sfarro_monitor (char* remount_dir)
{
    long initial_time = LONG_MAX;
    remount_directory=remount_dir;
    set_last_time_written (&initial_time);
    pthread_t tid;
    pthread_create (&tid, NULL, &threadproc, NULL);
    return;
}
