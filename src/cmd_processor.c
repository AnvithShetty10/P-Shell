/*
This file handles everything related to command string processing, such as, extracting commands and their arguments.
*/

/* -------------------- Includes -------------------- */
#include "cmd_processor.h"


/* -------------------- Statics and Globals -------------------- */

/* Error logger function for THIS file ONLY. */
#ifndef DEBUG_MODE
#define error_log(fmt, ...) ;
#endif
#ifdef DEBUG_MODE
static void error_log(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    dprintf(STDERR_FILENO, "\n");
    dprintf(STDERR_FILENO, "CMD PROCESSOR : ");
    vdprintf(STDERR_FILENO, fmt, args);
    dprintf(STDERR_FILENO, "\n");
    fflush(stderr);

    va_end(args);
}
#endif


/* -------------------- Function Definitions -------------------- */

char **parse_cmd_args(char *full_cmd) {
    char **ret = NULL;

    int count = 0, len = strlen(full_cmd) - 1;
    char *temp = full_cmd;
    error_log("len = %d; temp[len] = %c; *temp = %c", len, temp[len], *temp);
    while(temp[len] == ' ')
        temp[len--] = 0;
    while(*temp == ' ')             // strip spaces
        temp++;

    void *check = NULL;
    do {
        check = realloc(ret, sizeof(char *) * (count + 1));
        if(check)
            ret = check;
        else {
            perror("FAILED TO REALLOC TABLE OF CMD AND ARGS"); 
            return NULL;
        }

        ret[count++] = temp;
        
        // If string is given in quotes, remove the quotes
        int index, len = strlen(ret[count-1]), jindex;
        for(index = 0 ; index < len ; index++) {
            if(ret[count-1][index] == '"')      // Check for quotes "
            {
                for(jindex = index; jindex < (len - 1) ; jindex++)
                    ret[count-1][jindex] = ret[count-1][jindex + 1];      // move chars back, removing the quotes
                len--;      // reduce length
            }
        }

        ret[count-1][len] = 0;      // set new nul char to terminate string

        // Process env variables
        if(ret[count-1][0] == '$') {
            ret[count-1] = getenv(ret[count-1] + 1);
        }

        strsep(&temp, " \t");

    }while(temp);

    check = realloc(ret, sizeof(char *) * (++count));
    if(check)
        ret = check;
    else {
        perror("FAILED TO REALLOC TABLE OF CMD AND ARGS 2");
        return NULL;
    }

    ret[count - 1] = NULL;  // add final NULL


    // Printing all arguments for debugging
    #ifdef DEBUG_MODE
    int i = 0; temp = *(ret + i); while(temp) { error_log("CMDARGS %s", temp); temp = *(ret + ++i); }
    #endif

    error_log("Done parsing command and args!");
    return ret;
}


char ***parse_commands(char *full_cmd) {
    error_log("Entered parse_commands");
    char ***ret = NULL;

    int count = 0;
    char *temp = full_cmd, *temp_cmd = NULL;
    void *check = NULL;
    do {
        check = realloc(ret, sizeof(char **) * (count + 1));
        if(check)
            ret = check;
        else {
            perror("FAILED TO REALLOC TABLE OF COMMANDS");
            return NULL;
        }

        error_log("Before strsep on pipe; temp = %s", temp);
        temp_cmd = strsep(&temp, "|");
        error_log("strsep on pipe: temp_cmd = %s; temp = %s", temp_cmd, temp);

        ret[count++] = parse_cmd_args(temp_cmd);
    }while(temp);

    check = realloc(ret, sizeof(char **) * (count + 1));
    if(check)
        ret = check;
    else {
        perror("FAILED TO REALLOC TABLE OF COMMANDS");
        return NULL;
    }
    ret[count++] = NULL;

    #ifdef DEBUG_MODE
    error_log("Printing all commands for debugging"); int i = 0; char **temp_; temp_ = *(ret + i); while(temp_) { error_log("CMD %s", temp_[0]); temp_ = *(ret + ++i); }
    #endif

    error_log("Done parsing command!");
    return ret;
}