#include "remount.h"

int 
remount_device(char * remount_path)
{
    int res;
    unsigned long mountflags = MS_REMOUNT | O_RDONLY;
    res = mount(remount_path, remount_path, NULL, mountflags, NULL );
    if(res < 0)
        return errno;
    return res;
}

