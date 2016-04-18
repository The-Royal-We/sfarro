#include "monitor.h"

long MAX_TIME = LONG_MAX;
volatile int device_is_ro = 0;
// TODO: Add in error handling


void *
threadproc (void * vfs_d)
{
	struct vfs_state *vfs_data = (struct vfs_state*) vfs_d;
	fprintf(stderr, "Mount device is %s \n", vfs_data->mountdir);
    while (1)
    {
        fprintf(stderr, "Checking filesystem\n");
        sleep (PERIODIC_TIME_DELAY);
        if (ready_to_remount() && device_is_ro == 0)
        {
            fprintf(stderr, "Periodic mount check passed\n");
            fprintf(stderr, "Remounting mountpoint to READ_ONLY!\n");
            remount_device(vfs_data->mountdir, "ro");
            //RESET CLOCK
            set_last_time_written(&MAX_TIME);
        }
    }
    return 0;
}

void
init_sfarro_monitor (struct vfs_state *vfs_d)
{
    set_last_time_written (&MAX_TIME);
    pthread_t tid;
    pthread_create (&tid, NULL, &threadproc, (void *) vfs_d);
    return;
}

extern int
get_device_is_readonly(){
	return device_is_ro;
}

extern void
set_device_is_readonly(int val){
	device_is_ro = val;
}
