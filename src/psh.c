/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"


/* -------------------- Main -------------------- */
int main(int argc, char **argv) {
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);
    char *check_input = NULL;
    print_welcome_message();

    do {
        print_prompt();
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)

    }while(check_input);

    printf("\n");
    return 0;
}


/* -------------------- Function Definitions -------------------- */

void print_welcome_message() {
    printf("%s", "Welcome ");
    printf("%s", getlogin());
    printf(", to \n");

    printf("       _     _               _          _ _ \n _ __ (_)___| |_ __ _    ___| |__   ___| | |\n| '_ \\| / __| __/ _` |  / __| '_ \\ / _ \\ | |\n| |_) | \\__ \\ || (_| |  \\__ \\ | | |  __/ | |\n| .__/|_|___/\\__\\__,_|  |___/_| |_|\\___|_|_|\n|_|                                         \n");

    printf("\n\n\n");
    fflush(stdout);
    
    return;
}

void print_prompt() {
    char *temp = NULL;
    char *cwd = NULL;
    cwd = getcwd(cwd, 0);

    switch(_prompt_type) {
        case 0:
            temp = malloc(USERNAME_LIMIT + PATH_MAX);
            
            strcpy(temp, getlogin());
            strcat(temp, ":");
            strcat(temp, cwd);
            strcat(temp, " => \0");

            write(STDOUT_FILENO, temp, strlen(temp));
            break;
    }

    free(temp);
    free(cwd);
}