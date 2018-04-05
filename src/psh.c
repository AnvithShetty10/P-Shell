/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
This file delegates work to other functions from other files.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"
#include "pista_main.h"
#include "cmd_processor.h"


/* -------------------- Macros -------------------- */

#ifndef DEBUG_MODE                  // if not in DEBUG MODE, replace all calls to error_log with empty statements! Efficiency MAX!
#define error_log(fmt, ...) ;
#endif


/* -------------------- Function Prototypes -------------------- */
#ifdef DEBUG_MODE
static void error_log(char *fmt, ...);
#endif
void print_welcome_message();
void print_prompt();

int current=0;

/* ==================== Main ==================== */
int main(int argc, char **argv) {
    print_welcome_message();
    
    char *check_input = NULL;
    
    char ***commands = NULL;
    int len;
    char *cwd_buf = NULL;
    char *te_str = NULL;
    
    cwd_buf = getcwd(cwd_buf, 0);
    error_log("current : %s", cwd_buf);
    te_str = (char *)malloc(sizeof(char) * (strlen(cwd_buf) + 14));
    strcpy(te_str, "\"");
    strcat(te_str, cwd_buf);
    strcat(te_str,"/sal-te/te");
    strcat(te_str, "\"");
    
    strcpy(al[0].keys,"te");
	strcpy(al[0].values,te_str);

    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);
    do {
        print_prompt();
        strcpy(buf, "");
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        len = strlen(buf);
        current = (current + 1) % HISTORY_COUNT;
        if(strlen(buf)>0){
        write_history(buf,current,getpid());        
        }
        error_log("Input taken : %p %s %d %d %d", check_input, buf, len, len > 1, check_input && len > 1);
        if(check_input && len > 1) {
            error_log("Processing the command...");
            buf[len-1] = 0;     // remove trailing \n
            commands = parse_commands(buf);
            switch(pista_delegate(commands)) {
                case 1:
                    _prompt_type = !_prompt_type;
                    break;

                case 2:
                    check_input = NULL;
                    break;
                
                default:
                    strcpy(buf, "");
            }

            free(commands);    
        }
        
    }while(check_input);

    free(buf);
    printf("\n   ______                ____               __\n  / ____/___  ____  ____/ / /_  __  _____  / /\n / / __/ __ \\/ __ \\/ __  / __ \\/ / / / _ \\/ /\n/ /_/ / /_/ / /_/ / /_/ / /_/ / /_/ /  __/_/\n\\____/\\____/\\____/\\__,_/_.___/\\__, /\\___(_)\n                             /____/\n");
    return 0;
}


/* -------------------- Function Definitions -------------------- */

/* Error logger function for THIS file ONLY. */
#ifdef DEBUG_MODE
static void error_log(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    dprintf(STDERR_FILENO, "\n");
    dprintf(STDERR_FILENO, "MAIN PSH : ");
    vdprintf(STDERR_FILENO, fmt, args);
    dprintf(STDERR_FILENO, "\n");
    fflush(stderr);

    va_end(args);
}
#endif


void print_welcome_message() {
    system("cd sal-te && make compile");
    strcpy(histPath, getenv("HOME"));
    strcat(histPath, "/");
    strcat(histPath, HISTORY_FILE);
    
    getcwd(helpPath, PATH_MAX * sizeof(char));
    strcat(helpPath, "/");
    strcat(helpPath, HELP_FILE);

    printf("%s", "Welcome ");
    printf(BOLDCYAN "%s" RESET, getlogin());
    printf(", to \n");
    // CREDITS : sudo apt-get install -y figlet
    printf(BOLDGREEN "       _     _               _          _ _ \n _ __ (_)___| |_ __ _    ___| |__   ___| | |\n| '_ \\| / __| __/ _` |  / __| '_ \\ / _ \\ | |\n| |_) | \\__ \\ || (_| |  \\__ \\ | | |  __/ | |\n| .__/|_|___/\\__\\__,_|  |___/_| |_|\\___|_|_|\n|_|                                         \n" RESET);
    /*if(fork() == 0) {
        close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);
        system("curl -X POST 13.126.134.245:5000/checkshellperson/$(whoami)");
        _exit(0);
    }*/

    printf("\n\n\n");
    fflush(stdout);
    
    return;
}


void print_prompt() {
    fflush(stdout);
    char *temp = NULL;
    char *dirc =NULL, *dname =NULL;
    char *cwd = NULL;
    cwd = getcwd(cwd, 0);
    temp = malloc(USERNAME_LIMIT + PATH_MAX);

    // Constructing the prompt...
    switch(_prompt_type) {
        case 0:
        
            strcpy(temp, getlogin());
            strcat(temp, ":");
            strcat(temp, cwd);
            strcat(temp, " => \0");

            printf(GREEN "%s" RESET, temp);
            break;
            
        case 1: 
        	
        	strcpy(temp, cwd);
            strcat(temp, "-->> \0");
            dirc = strdup(temp);
            dname = basename(dirc);
            
		
            printf(GREEN "%s" RESET, dname);
        	break;
    }

    fflush(stdout);
    free(temp);
    free(cwd);
}
