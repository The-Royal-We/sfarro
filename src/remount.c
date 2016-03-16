#include "remount.h"

extern int
remount_device(const char * remount_path, const char * remount_opt)
{
    int res;

    char* remount_call;
    char* space;
    space = " ";
    remount_call = "sudo mount --options=remount,";
    char* system_call = (char *) malloc (1 + strlen(remount_call)
    		+ strlen(remount_opt) + strlen(space) + strlen(remount_path));
    strcpy(system_call, remount_call);
    strcat(system_call, remount_opt);
    strcat(system_call, space);
    strcat(system_call, remount_path );
    fprintf(stderr, "System call is: %s\n", system_call);
    res = system(system_call);
  	fprintf(stderr, "mount completed with: %d\n", res);
    if(res < 0){
    	fprintf(stderr,"Remount failed with errno: %d\n", errno);
        return errno;
    }
    return res;
}
