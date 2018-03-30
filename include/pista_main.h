#ifndef PISTA_MAIN_H
#define PISTA_MAIN_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>


/* -------------------- System Libraries -------------------- */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>

/* -------------------- Macros -------------------- */
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define READ_END 0      /* read end for unnamed pipes */
#define WRITE_END 1     /* write end for unnamed pipes */

#define digit_to_char(d) (48+d)	/*convert digits to char for itoa*/

/* -------------------- Globals -------------------- */
struct alias
{
	char keys[2];
	char values[20];
}al[100];

char histPath[PATH_MAX];

/* -------------------- Function Prototypes -------------------- */

/*
Process pista's command's!
*/
int pista_command(char **cmd_args);

/*
Returns the index of the key in the alias table. If not found, returns -1
*/
int is_alias(char *key);

/*
Process all commands, delegate pista's in-built commands to pista_command, fork and exec the rest. Handles pipes, redirections, and child processes.
--------------------------------|
Return  | Command               |
value   |                       |
--------------------------------|
0       | not pista command!    |
1       | chprompt              |
2       | exit                  |
3       | cd                    |
4       | export                |
5		| help                  |
6		| history				|
7		| alias 				|
--------------------------------
*/
int pista_delegate(char ***commands);

char **handle_redirections(char **cmd_args);

/*
Spawns child process to execute command. Also set's up it's STDIN and STDOUT using dups.
*/
void spawn_child_cmd(char **cmd_args, int instate, int fdin, int outstate, int fdout, int **pipes, int *children, int curr);

int write_history(char *,int, pid_t);
#endif