/*
This file has functions that assist command execution, such as dealing with environment variables.
*/

/* -------------------- Includes -------------------- */
#include "cmd_helper.h"


/* -------------------- Statics and Globals -------------------- */

/* Error logger function for THIS file ONLY. */
static void error_log(char *fmt, ...) {
#ifdef DEBUG_MODE
    va_list args;
    va_start(args, fmt);

    printf("\n");
    printf("CMD HELPER : ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
#endif
}


/* -------------------- Function Definitions -------------------- */