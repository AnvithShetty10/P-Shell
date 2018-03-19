#ifndef CMD_HELPER_H
#define CMD_HELPER_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* -------------------- System Libraries -------------------- */
#include <unistd.h>
#include <fcntl.h>
#include<limits.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/wait.h>


/* -------------------- Macros -------------------- */



/* -------------------- Globals -------------------- */



/* -------------------- Function Prototypes -------------------- */

/*
Returns a 2D array of paths to search for executables; usually from environemnt variable.
Last entry is always NULL.
*/
char **paths();


#endif