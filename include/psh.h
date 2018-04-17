#ifndef PSH_H
#define PSH_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

/* -------------------- System Libraries -------------------- */
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <libgen.h>


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


#define USERNAME_LIMIT 256          // Default username length limit 256 bytes
#define BUF_SIZE_LIMIT 256000       // Default buffer size limit 256 KB

#define HISTORY_COUNT 25
#define HISTORY_FILE "history.txt"

#define HELP_FILE "help.txt"

#define getlogin() getpwuid(getuid())->pw_name


/* -------------------- Globals -------------------- */
int _prompt_type = 0;
int hist_cmd_count;

/* -------------------- Function Prototypes -------------------- */
void execute_command(char *);
void print_welcome_message();
void print_prompt();
int count_history();

#endif

