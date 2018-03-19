#ifndef DIR_STUFF_H
#define DIR_STUFF_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* -------------------- System Libraries -------------------- */
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include<limits.h>
#include <sys/dir.h>
#include <sys/wait.h>

/* -------------------- Macros -------------------- */



/* -------------------- Globals -------------------- */



/* -------------------- Function Prototypes -------------------- */

/*
Searches for `file` in `path`; returns 1 if present, else 0.
`path` must be the path to a directory!
*/
uint8_t file_exists(char *path, char *file);


#endif