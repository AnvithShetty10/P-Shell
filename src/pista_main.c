/*
This file contains the functions of Pista shell itself. `psh` delegates work to this program.
*/

/* -------------------- Includes -------------------- */
#include "pista_main.h"


/* Error logger function for THIS file ONLY. */
static void error_log(char *fmt, ...) {
#ifdef DEBUG_MODE
    va_list args;
    va_start(args, fmt);

    printf("\n");
    printf("PISTA MAIN : ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
#endif
}

/* -------------------- Function Definitions -------------------- */

int pista_command(char **cmd_args) {
    char *temp = NULL;

    // CHPROMPT IS 1
    if (strcmp(cmd_args[0], "chprompt") == 0) {
        error_log("CHPROMPT matched!");
        return 1;
    }
    // EXIT IS 2
    else if (strcmp(cmd_args[0], "exit") == 0) {
        error_log("Exit matched!");

        printf(BOLDRED "Are you sure (Y/N) ?  " RESET);
        switch(getchar()) {
            case 'Y': case 'y':
                break;
            
            case 'N': case 'n':
                return 0;

            default:
                printf(RED "Invalid choice!" RESET);
                return 0;
        }
        
        error_log("Exiting!");
        return 2;
    }
    // CD IS 3
    else if (!strcmp(cmd_args[0], "cd")) {
        error_log("CD matched!");

        char *gdir = NULL, *dir = NULL, *to = NULL;

        if (cmd_args[1]) {
            gdir = getcwd(gdir, 0);
            temp = realloc(gdir, sizeof(char) * (strlen(gdir) + strlen(cmd_args[1]) + 2));
            if (temp) {
                error_log("Successful realloc of GDIR");
                gdir = temp;
            }
            else {
                perror("FAILED TO REALLOC GDIR");
                return 0;
            }
            dir = strcat(gdir, "/");
            to = strcat(dir, cmd_args[1]);
        }
        else {
            error_log("CD without args!");
            to = getenv("HOME");
        }

        chdir(to);
        free(gdir);
        
        return 3;
    }
    // EXPORT IS 4
    else if (!strcmp(cmd_args[0], "export")) {
        error_log("EXPORT matched!");
        int index, temp = 0;
        for(index = 1 ; cmd_args[index] ; index++) {
            temp = putenv(strdup(cmd_args[index]));
            if(temp < 0) {
                error_log("Put %s", cmd_args[index]);
                perror("export error");
                error_log("errno = %d", errno);
            }
        }
        
        return 4;
    }

    // NO PISTA COMMAND!
    return 0;
}


int pista_delegate(char **cmd_args) {
    pid_t pid;
    int status = 0;
    
    pid = fork();
    if(pid < 0) {
        perror("fork error");
    }
    else if(pid == 0) {
        int check_exec = execvp(cmd_args[0], cmd_args);

        error_log("CHILD Print post exec! THIS SHOULD NOT HAPPEN! errno = %d", errno);
        if(check_exec < 0) {
            #ifdef DEBUG_MODE 
            perror("CHILD EXECVP ERROR"); 
            #endif
            printf("%s : No such command or executable!\n", cmd_args[0]);
        }
        _exit(0);
    }
    else {
        wait4(pid, &status, 0, NULL);
        error_log("Done waiting!");
        fflush(stdout);
    }

    return status;
}