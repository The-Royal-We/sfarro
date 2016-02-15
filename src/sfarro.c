#include "vfs.h"

int
main (int argc, char *argv[])
{
  if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
    {
      sfarro_usage ();
      return -1;
    }
    init_sfarro_monitor();
  return vfs (argc, argv);
}

void
sfarro_usage ()
{
  fprintf (stderr,
	   "usage:  src [FUSE and mount options] rootDir mountPoint\n");
}
