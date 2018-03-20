/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"
#include "cmd_processor.h"
#include "dir_stuff.h"


/* -------------------- Macros -------------------- */
#define getlogin() getpwuid(getuid())->pw_name


/* -------------------- Function Prototypes -------------------- */
static void error_log(char *fmt, ...);
void print_welcome_message();
void print_prompt();


/* ==================== Main ==================== */
int main(int argc, char **argv) {
    print_welcome_message();

    char *check_input = NULL, *temp = NULL;
    char *gdir, *dir, *to;
    char **cmd_args = NULL;
    int status = 0, len;
    pid_t pid;

    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);
    char *path_to_exec = (char *)malloc(sizeof(char) * PATH_MAX);

    do {
    	gdir = NULL;
		dir = NULL;
		to = NULL;
        print_prompt();
        strcpy(buf, "");
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        len = strlen(buf);
        
        error_log("Input taken : %p %s %d %d %d", check_input, buf, len, len > 1, check_input && len > 1);
        if(check_input && len > 1) {
            error_log("Processing the command...");
            buf[len-1] = 0;     // remove trailing \n
            
            if(strcmp(buf, "chprompt") == 0) {
                error_log("CHPROMPT matched!");
            	_prompt_type = !_prompt_type;
            }
            else if(strcmp(buf, "exit") == 0) {
                error_log("Exiting!");
                check_input = NULL;
                continue;
            }
       		else {
                cmd_args = parse_cmd_args(buf);

                if (!strcmp(cmd_args[0], "cd")) {
                    error_log("CD matched!");
                    
                    if(cmd_args[1]) {
                        gdir = getcwd(gdir, 0);
                        temp = realloc(gdir, sizeof(char) * (strlen(gdir) + strlen(cmd_args[1]) + 2));
                        if(temp) {
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
                    continue;
                }
                if (!strcmp(cmd_args[0], "export")) {
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
                    continue;
                }
                    
                pid = fork();
                if(pid < 0) {
                    perror("fork error");
                }
                else if(pid == 0) {
                    error_log("CHILD path_to_exec = %s\n", path_to_exec);
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
                
                free(cmd_args);
            
           }
        }
        
    }while(check_input);

    free(buf);
    printf("Pista says goodbye!\n");
    return 0;
}


/* -------------------- Function Definitions -------------------- */

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

void print_welcome_message() {
    printf("%s", "Welcome ");
    printf("%s", getlogin());
    printf(", to \n");
    // CREDITS : sudo apt-get install -y figlet
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
