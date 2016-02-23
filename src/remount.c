/**
 * We run a timed function call that periodically checks if the vfs can be remounted safely
 * We then call system library function: remount(2) on the vfs.
 */

#include "remount.h"

int
remount (const char *remount_directory)
{
  int res;
  if (access (remount_directory, W_OK | R_OK) == 0)
    {
      res = mount(remount_directory, remount_directory, NULL, MS_REMOUNT | O_RDONLY, NULL);
      fprintf (stderr, "**** Remount occured at %s, with result %d ***** \n",
	      remount_directory, res);
      if (res < 0)
      {
    	printf ("Remount failed. Error number: %d !!! \n", errno);
      }
    }
  else
    {
      res = -1;
    }
  return res;
}
