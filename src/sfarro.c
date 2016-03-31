#include "sfarro.h"

int
main (int argc, char *argv[])
{ 
  int sfarro_stat;
  struct vfs_state *vfs_data;

  if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
    {
      sfarro_usage ();
      return -1;
    }

  vfs_data = malloc (sizeof (struct vfs_state));

  if (vfs_data == NULL)
    {
      perror ("main calloc");
      abort ();
    }

  vfs_data->mountdir = realpath(argv[argc-2], NULL);
  argv[argc-2] = argv[argc-1];
  argv[argc-1] = NULL;
  argc--;

  vfs_data->rootdir = argv[argc-1];

  fprintf (stderr, "Initialising monitor system.\n");
  init_sfarro_monitor(vfs_data);

  fprintf (stderr, "Calling writable filesystem.\n");

  sfarro_stat = initialize_vfs(argc, argv, vfs_data);
  return sfarro_stat;

}

void
sfarro_usage ()
{
  fprintf (stderr,
	   "usage:  ./sfarro -d [FUSE and mount options] mountDirectory temporaryMirrorDirectory\n");
}
