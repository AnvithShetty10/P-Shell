/*
This file contains the functions of Pista shell itself. `psh` delegates work to this program.
*/

/* -------------------- Includes -------------------- */
#include "pista_main.h"


/* -------------------- Function Definitions -------------------- */

/* Error logger function for THIS file ONLY. */
#ifndef DEBUG_MODE
#define error_log(fmt, ...) ;
#endif
#ifdef DEBUG_MODE
static void error_log(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    dprintf(STDERR_FILENO, "\n");
    dprintf(STDERR_FILENO, "PISTA MAIN : ");
    vdprintf(STDERR_FILENO, fmt, args);
    dprintf(STDERR_FILENO, "\n");
    fflush(stderr);

    va_end(args);
}
#endif


int pista_command(char **cmd_args) {
    char *temp = NULL;

    // CHPROMPT IS 1
    if (strcmp(cmd_args[0], "chprompt") == 0) {
        error_log("CHPROMPT matched!");
        error_log("PISTA COMMAND 1!");
        return 1;
    }
    // EXIT IS 2
    else if (strcmp(cmd_args[0], "exit") == 0) {
        error_log("Exit matched!");

        printf(BOLDRED "Are you sure (Y/N) ?  " RESET);
        fflush(stdout);
        switch(getchar()) {
            case 'Y': case 'y':
                break;
            
            case 'N': case 'n':
                return 0;

            default:
                printf(RED "Invalid choice!" RESET);
                return 0;
        }
        
        error_log("PISTA COMMAND 2!");
        return 2;
    }
    // CD IS 3
    else if (!strcmp(cmd_args[0], "cd")) {
        error_log("CD matched!");

        char *gdir = NULL, *dir = NULL, *to = NULL;

        if (cmd_args[1]) {
            gdir = getcwd(gdir, 0);
            if(gdir == NULL) { perror("GETCWD FAIL"); error_log("GDIR = %s", gdir); }

            temp = realloc(gdir, sizeof(char) * (strlen(gdir) + strlen(cmd_args[1]) + 2));
            if (temp) {
                gdir = temp;
            }
            else {
                perror("REALLOC FAIL");
                return 0;
            }

            dir = strcat(gdir, "/");
            to = strcat(dir, cmd_args[1]);
        }
        else {
            error_log("CD without args!");
            to = getenv("HOME");
            if(to == NULL) perror("HOME enviornment variable not set!");
        }

        if(chdir(to) < 0) {
            fprintf(stderr, BOLDRED "CD ERROR : " RESET);
            perror("");
        }
        free(gdir);
        
        error_log("PISTA COMMAND 3!");
        return 3;
    }
    // EXPORT IS 4
    else if (!strcmp(cmd_args[0], "export")) {
        error_log("EXPORT matched!");
        int index, temp = 0;
        for(index = 1 ; cmd_args[index] ; index++) {
            temp = putenv(strdup(cmd_args[index]));
            if(temp != 0) { error_log("Put %s", cmd_args[index]); perror("export error"); }
        }
        
        error_log("PISTA COMMAND 4!");
        return 4;
    }

    // NO PISTA COMMAND!
    error_log("PISTA COMMAND 0!");
    return 0;
}


int pista_delegate(char ***commands) {
    error_log("Entered pista delegate");

    pid_t pid, chpid;
    int status = 0, i = 0, j, k, temp;
    int **pipes = NULL, *children = NULL;
    char **cmd_args;
    void *check_reallocs = NULL;
    
    // Save STDIN STDOUT
    int savedin = dup(STDIN_FILENO);
    if(savedin < 0) { perror("DUP savedin"); _exit(0); }

    int savedout = dup(STDOUT_FILENO);
    if(savedout < 0) { perror("DUP savedout"); _exit(0); }

    char *infile = NULL, *outfile = NULL;    // Redirected IP or OP 
    int fdin, fdout;                        // child's stdin and stdout
    int instate = 0, outstate = 0;          // so child knows what to dup

    // Process all commands!
    while(commands[i] != NULL) {
        cmd_args = commands[i];
        //i++;  // moved to end of while loop to avoid confusion!

        check_reallocs = realloc(pipes, sizeof(char *) * i);    // allocate pointer to store pipe
        if(check_reallocs) pipes = check_reallocs;
        else { perror("PIPES REALLOC ERROR"); _exit(errno); }


        pipes[i] = (int *)malloc(sizeof(int) * 2);      // allocate space for the actual pipe


        check_reallocs = realloc(children, sizeof(char *) * i);    // extend children pointer
        if(check_reallocs) children = check_reallocs;
        else { perror("PIPES REALLOC ERROR"); _exit(errno); }


        if( !(temp = pista_command(cmd_args)) ) {
            if(i == 0) {    // if first command
                error_log("First child!");
                
                // handle input
                if(infile) {
                    instate = 1;
                    ;   // open input file! Check for input redirection here!
                }
                else {
                    instate = 2;
                    fdin = dup(STDIN_FILENO);
                }
                
                // handle output
                if(commands[i+1] != NULL) { // if there are more children
                    outstate = 1;
                    pipe(pipes[i]);           // create the pipe
                    fdout = pipes[i][WRITE_END];
                }
                else {
                    outstate = 2;
                    fdout = dup(STDOUT_FILENO);
                }
            }
            else if(commands[i+1] == NULL) {   //if last command
                error_log("last command in sequence");
                
                // handle input
                instate = 3;
                fdin = pipes[i-1][READ_END];

                // handle output
                if(outfile) {
                    outstate = 3;
                    ;   // open output file! OUTPUT redirection done here!
                }
                else {
                    outstate = 4;
                    fdout = dup(STDOUT_FILENO);
                }
            }
            else {
                error_log("not first or last command in sequence");

                instate = 4;
                outstate = 5;
                pipe(pipes[i]);     // create our pipe
                
                fdin = pipes[i-1][READ_END];
                fdout = pipes[i][WRITE_END];
            }

            error_log("FORKING instate = %d\toutstate = %d", instate, outstate);

            pid = fork();
            if(pid < 0) {
                perror("fork error");
            }
            else if(pid == 0) {
                dup2(fdin, STDIN_FILENO);
                switch(instate) {
                    case 1: case 2: 
                        break;

                    case 3: case 4:
                        close(pipes[i-1][WRITE_END]);
                        break;
                }

                dup2(fdout, STDOUT_FILENO);
                switch(outstate) {
                    case 1: case 5:
                        close(pipes[i][READ_END]);
                        break;

                    case 2: case 3: case 4: 
                        break;
                }

                int check_exec = execvp(cmd_args[0], cmd_args);

                error_log("CHILD Print post exec! THIS SHOULD NOT HAPPEN! errno = %d", errno);
                if(check_exec < 0) {
                    #ifdef DEBUG_MODE 
                    perror("CHILD EXECVP ERROR");
                    #endif
                    printf(RED "%s : No such command or executable in $PATH!\n" RESET, cmd_args[0]);
                }
                
                exit(0);
            }
            else {
                error_log("pid = %d", pid);
                children[i] = pid;
            }
        }
        else 
            return temp;
        
        i++;    // loop variable! Important!
    }

    
    error_log("Pista going to wait for children");
    int flag = 0;
    for(j = 0 ; j < i ; j++) {
        flag = 0;
        while( (chpid = waitpid(-1, &status, WUNTRACED | WNOHANG)) < 0) ;
        fflush(stdout); fflush(stderr);

        if(WIFEXITED(status))
            error_log("%d exited with %d", chpid, WEXITSTATUS(status));

        for(k = 0 ; k < i ; k++) {
            if(children[k] == chpid) {
                flag = !flag;
                error_log("child k = %d", k);
                close(pipes[k][WRITE_END]);
                break;
            }
        }

        if(!flag)   // if non-forked child pid found, ignore it!
            j--;
    }
    
    error_log("Done waiting!");
    
    error_log("Restoring default FDs");
    dup2(savedin, STDIN_FILENO);
    dup2(savedout, STDOUT_FILENO);
    close(savedin);
    close(savedout);

    free(pipes);
    free(children);

    return status;
}
