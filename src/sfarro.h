/** @file
* This file defines the entry point for SFARRO
*/

#ifndef SFARRO_H
#define SFARRO_H
#include <stdio.h>

#include "vfs.h"
#include "params.h"
#include "constants.h"

#endif //SFARRO_H

/**
* Creates our remount monitor thread and initalizes our
* safe filsystem.
* 
* @param Command line arguments
* @return status from initialize_vfs
*/
int main(int argc, char *argv[]);

/**
* Prints out the default usage for sfarro if error has been made
* when entering inputs.
*/
void sfarro_usage();
