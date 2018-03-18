/*
This file has functions that assist command execution, such as dealing with environment variables.
*/

#include "cmd_helper.h"

char **paths() {
    char **path_table = (char **)malloc(sizeof(char *) * 3);
    path_table[0] = strdup("/usr/bin");
    path_table[1] = strdup("/bin/");
    path_table[2] = NULL;

    return path_table;
}