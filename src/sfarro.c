#include "sfarro.h"

int
main (int argc, char *argv[])
{ 
  int sfarro_status;
  struct vfs_state *vfs_data;

  if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
    {
      sfarro_usage ();
      return -1;
    }
  if ((getuid () == 0) || (geteuid () == 0))
    {
      fprintf (stderr, "Running sfarro as root opens security holes\n");
      fprintf (
           stderr,
	       "As in possibly blowing up a your machine if root is mounted \n"
           );
      return -1;
    }

  vfs_data = malloc (sizeof (struct vfs_state));

  if (vfs_data == NULL)
    {
      perror ("main calloc");
      abort ();
    }

  /*
   * Pull the root directory from the argument list and save it in my internal data
   */


  vfs_data->rootdir = realpath(argv[argc-2], NULL);
  argv[argc-2] = argv[argc-1];
  argv[argc-1] = NULL;
  argc--;

  fprintf (stderr, "Initialising monitor system.\n");
  init_sfarro_monitor(realpath(argv[argc-1], NULL));

  fprintf (stderr, "Calling writable filesystem.\n");
  sfarro_status = vfs (argc, argv, vfs_data);
  return sfarro_status;
}

void
sfarro_usage ()
{
  fprintf (stderr,
	   "usage:  ./sfarro [FUSE and mount options] rootDir mountPoint\n");
}
