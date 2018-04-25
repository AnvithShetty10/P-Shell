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
char *check_input = NULL;    
char ***commands = NULL;
/* ==================== Main ==================== */
int main(int argc, char **argv) {
    shell_pid = getpid();
    setup_signals();
    print_welcome_message();
    hist_cmd_count=count_history();
        
    
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
        //check_input = fgets(buf, BUF_SIZE_LIMIT, stdin);    // returns NULL if EOF is found (CTRL-D)
        error_log("buf is %p", buf);
        check_input = processor(buf);
        len = strlen(buf);
        current = (current + 1) % HISTORY_COUNT;
        if(strlen(buf)>0 && _prompt_type < 3){
        write_history(buf,current,getpid());        
        }
        error_log("Input taken : %p %s %d %d %d", check_input, buf, len, len > 0, check_input && len > 0);
        if(check_input && len > 0) {
            error_log("Processing the command...");
            //buf[len-1] = 0;     // remove trailing \n
            execute_command(check_input);    
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

void execute_command(char *buf){
            
            commands = parse_commands(buf);
            switch(pista_delegate(commands)) {
                case 1:
                    switch(_prompt_type) {
                        case 0: case 1:
                            _prompt_type = !_prompt_type;
                            break;
                        case 3:
                            _prompt_type++;
                            break;

                        case 4:
                            _prompt_type--;
                            break;
                    }
                    break;

                case 2:
                    check_input = NULL;
                    break;

                case 9:
                    if(_prompt_type < 3)
                        _prompt_type = _prompt_type + 3;
                    else
                        _prompt_type = _prompt_type - 3;
                
                default:
                    strcpy(buf, "");
            }

            //free(commands);
}

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

int count_history(){
    FILE *fp = fopen(histPath,"r");
    if(fp == NULL)
        return 0;
    int lines=0;
    char ch;
    while(!feof(fp))
    {
      ch = fgetc(fp);
      if(ch == '\n')
      {
        lines++;
      }
    }
    return lines;
}

void print_prompt() {
    //printf("\n");
    fflush(stdout);
    char *temp = NULL;
    char *dirc =NULL, *dname =NULL;
    char *cwd = NULL;
    cwd = getcwd(cwd, 0);
    temp = malloc(USERNAME_LIMIT + PATH_MAX);

    // Constructing the prompt...
    switch(_prompt_type) {
        case 0: case 3:
        
            strcpy(temp, getlogin());
            strcat(temp, ":");
            strcat(temp, cwd);
            strcat(temp, " => \0");
            if(_prompt_type == 0)
                printf(GREEN "%s" RESET, temp);
            if(_prompt_type == 3)
                printf(BOLDWHITE "%s" RESET, temp);
            break;
            
        case 1: case 4:
        	
        	strcpy(temp, cwd);
            strcat(temp, "-->> \0");
            dirc = strdup(temp);
            dname = basename(dirc);
            if(_prompt_type == 1)
                printf(GREEN "%s" RESET, dname);
            if(_prompt_type == 4)
                printf(BOLDWHITE "%s" RESET, dname);
        	break;
    }

    fflush(stdout);
    free(temp);
    free(cwd);
}
