#include "monitor.h"

void init_sfarro_monitor();

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
      sleep (PERIODIC_TIME_DELAY);
      if (periodic_remount_check () == 1)
	{
	  if (remount (VFS_DATA->rootdir) != 0)
	    {
	      return 0;
	    }
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
