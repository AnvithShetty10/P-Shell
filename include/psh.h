#ifndef PSH_H
#define PSH_H

/* -------------------- Standard Libraries -------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
#define USERNAME_LIMIT 256          // Default username length limit 256 bytes
#define BUF_SIZE_LIMIT 256000       // Default buffer size limit 256 KB


/* -------------------- Globals -------------------- */
int _prompt_type = 0;


/* -------------------- Function Prototypes -------------------- */
void print_welcome_message();
void print_prompt();

#endif
