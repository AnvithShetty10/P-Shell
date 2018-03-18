#ifndef CMD_PRO_H
#define CMD_PRO_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/* -------------------- Globals -------------------- */



/* -------------------- Function Prototypes -------------------- */

/*
Parses a null-terminated string and splits it at every space. 
The last entry in the 2D array that is returned is NULL; makes it easy to use with execvp syscall.
*/
char **parse_cmd_args(char *full_cmd);


#endif