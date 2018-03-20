/*
This file handles everything related to command string processing, such as, extracting commands and their arguments.
*/

/* -------------------- Includes -------------------- */
#include "cmd_processor.h"


/* -------------------- Statics and Globals -------------------- */

/* Error logger function for THIS file ONLY. */
static void error_log(char *fmt, ...) {
#ifdef DEBUG_MODE
    va_list args;
    va_start(args, fmt);

    printf("\n");
    printf("CMD PROCESSOR : ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
#endif
}


/* -------------------- Function Definitions -------------------- */

char **parse_cmd_args(char *full_cmd) {
    char **ret = NULL;

    int count = 0;
    char *temp = full_cmd;
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
        strsep(&temp, " ");

    }while(temp);

    check = realloc(ret, sizeof(char *) * (++count));
    if(check)
        ret = check;
    else {
        perror("FAILED TO REALLOC TABLE OF CMD AND ARGS 2");
        return NULL;
    }

    ret[count - 1] = NULL;

    // Printing all arguments for debugging
    #ifdef DEBUG_MODE
    int i = 0;
    temp = *(ret + i);
    while(temp) {
        error_log("%s", temp);
        temp = *(ret + ++i);
    }
    #endif

    error_log("Done parsing command and args!");
    return ret;
}