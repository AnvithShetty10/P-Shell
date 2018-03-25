#ifndef CMD_PRO_H
#define CMD_PRO_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/wait.h>

/* -------------------- Globals -------------------- */



/* -------------------- Function Prototypes -------------------- */

/*
Parses a null-terminated string and splits it at every space or tab. 
The last entry in the 2D array that is returned is NULL; makes it easy to use with execvp syscall.
*/
char **parse_cmd_args(char *full_cmd);

/*
Parses a null-terminated string and splits it at every pipe. Then calls parse_cmd_args and gets 2D arrays of command and args.
The last entry in the 3D array that is returned is NULL; makes it easy to use.
*/
char ***parse_commands(char *full_cmd);

#endif