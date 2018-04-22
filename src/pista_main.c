/*
This file contains the functions of Pista shell itself. `psh` delegates work to this program.
*/

/* -------------------- Includes -------------------- */
#include "pista_main.h"
#include "cmd_processor.h"
int _a = 1;
int timed_cmd = 0;


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

char *itoa(int i) {
    error_log("Entered ITOA with %d", i);

    char *ret = (char *)malloc(sizeof(char));
    int curr = 0;
    while(i) {
        ret = (char *)realloc(ret, (curr + 2) * sizeof(char));
        ret[curr++] = digit_to_char(i % 10);
        i /= 10;
    }
    ret[curr] = 0;

    error_log("Reversing %d; %d; %s", curr, curr/2, ret);
    // reverse
    int j; 
    char temp;
    for(i = 0, j = curr-1 ; i < (curr / 2) ; i++, j--) {
        temp = ret[i];
        ret[i] = ret[j];
        ret[j] = temp;
    }
    
    error_log("Returning ITOA : %s", ret);
    return ret;
}

extern int hist_cmd_count;
extern int command_index;
int reverse_flag=0;
int write_history(char *buf,int current,pid_t pid){
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int j=0;
    char* pid_str=NULL;
    pid_str=itoa(pid);
    for(j=0;j<25;j++)
        strcpy(command[j],"");
        strcpy(info[j],"");
    int fd=open(histPath,O_RDWR|O_APPEND|O_CREAT,0666);
    if(strlen(buf)>0)
        {
         strcpy(info[current],pid_str);   
         strcat(info[current],"  ");    
         strcat(info[current],asctime(tm));        
         strcpy(command[current],"  ");
         strcat(command[current],buf);
         if (info[current][strlen(info[current])-1]=='\n') info[current][strlen(info[current])-1]='\0';     
         strcat(info[current],command[current]);

        }	  
    if(fd){
            write(fd,info[current],strlen(info[current])+1);
            hist_cmd_count++;
            reverse_flag=1;
            command_index = 0;
            write(fd,"\n",1);
        }
    close(fd);    
    return 0;
}


int pista_command(char **cmd_args) {
    char *temp = NULL;
    char *temp_al_val=cmd_args[1];
    char *temp_al_key=NULL;
    timed_cmd = 0;

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
        char opt[100];
        fgets(opt, 99, stdin);
        switch(opt[0]) {
            case 'Y': case 'y':
                break;
            
            case 'N': case 'n':
                return -1;

            default:
                printf(RED "Invalid choice!\n" RESET);
                return -1;
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

    // HELP IS 5
    else if (!strcmp(cmd_args[0], "help")) {
        error_log("HELP matched!");
        char str;
        int fd=open(helpPath,O_RDONLY);
        if(fd){
            while(read(fd,&str,sizeof(str)))
                printf("%c",str);
            }
        close(fd);    
        
        error_log("PISTA COMMAND 5!");
        return 5;
    }    

    // HISTORY IS 6
    else if (!strcmp(cmd_args[0], "history")) {
        error_log("HISTORY matched!");
        char str;
        int fd=open(histPath,O_RDWR);
        if(fd){
            while(read(fd,&str,sizeof(str)))
                printf("%c",str);
            }
        close(fd);    
        error_log("PISTA COMMAND 6!");
        return 6;
    }

    // ALIAS IS 7
    
    else if (!strcmp(cmd_args[0], "alias")) {
        error_log("ALIAS matched!");
        if(cmd_args[1]) {
            temp_al_key=strsep(&temp_al_val, "=");
            strcpy(al[_a].keys, temp_al_key);
            strcpy(al[_a].values, temp_al_val);
            _a++;
        }
        else {
            int k=0;
            for(k=0;k<_a;k++){
                printf("key = %s\n",al[k].keys );
                printf("val = %s\n\n",al[k].values );
            }
        }
         
        error_log("PISTA COMMAND 7!");
        return 7;
    }

	 // HC i.e. HISTORY CLEAR IS 8
    else if (!strcmp(cmd_args[0], "hc")) {
        error_log("HC matched!");
        remove(histPath);   
        hist_cmd_count=0;
        error_log("PISTA COMMAND 8!");
        return 8;
    }    
    else if (!strcmp(cmd_args[0], "timeout")) {
        error_log("timeout matched!");
        int timeoutVal = cmd_args[1] != NULL ? atoi(cmd_args[1]) : 0;
        error_log("timeout %d", timeoutVal);
        if(timeoutVal > 0) {
            int t = 0;
            while(cmd_args[t+2] != NULL) {
                cmd_args[t] = cmd_args[t+2];
                t++;
            }
            cmd_args[t] = NULL;
            timed_cmd = timeoutVal;
        }
        else {
            int t = 0;
            while(cmd_args[t+1] != NULL) {
                cmd_args[t] = cmd_args[t+1];
                t++;
            }
            cmd_args[t] = NULL;
        }
    }
    else if (!strcmp(cmd_args[0], "incog")) {
        error_log("incog matched!");
        error_log("PISTA COMMAND 9!");
        return 9;
    }
    
    // wildcard * or ? at end
    else if (cmd_args[1] != NULL) {
        error_log("Going to check for wildcard");
        if(!strcmp(cmd_args[0], "ls") && ((cmd_args[1][strlen(cmd_args[1])-1]=='*') || (cmd_args[1][strlen(cmd_args[1])-1]=='?'))) {
    	
    		char rem[20];
			int q=0, k=0, p=0;
			char temp_fname[30];
			int fname_len=0;
			int cmd_len=0, rem_len=0;
			
			cmd_len=strlen(cmd_args[1]);
    		char *path=".";
			DIR *dirp=opendir(path);
			struct dirent *dp;
	
			for(q=0; q<cmd_len-1; q++)
			{
				rem[k++]=cmd_args[1][q];
			}
			rem_len=strlen(rem);
			
			if(cmd_args[1][strlen(cmd_args[1])-1]=='*')
			{
				while((dp=readdir(dirp)) != NULL)
				{
					int ctr=0;
					strcpy(temp_fname, dp->d_name);
			
				
					for(p=0; p<rem_len; p++)
					{
						if(temp_fname[p]==rem[p])
						{
							ctr++;
						}
					}
				
					if(ctr==rem_len)
					{
						printf("%s\n",dp->d_name);
					}
				}	
			}
			
			else if(cmd_args[1][strlen(cmd_args[1])-1]=='?')
			{
				while((dp=readdir(dirp)) != NULL)
				{
					int ctr2=0;
					strcpy(temp_fname, dp->d_name);
					fname_len=strlen(temp_fname);
				
					for(p=0; p<rem_len; p++)
					{
						if(temp_fname[p]==rem[p])
						{
							ctr2++;
						}
					}
				
					if((ctr2==rem_len) && (fname_len==(rem_len+1)))
					{
						printf("%s\n",dp->d_name);
					}
				}	
			}
			
       		error_log("Wildcard matched!");
        }
       	 // wildcard * or ? at beginning
    else if (!strcmp(cmd_args[0], "ls") && ((cmd_args[1][0]=='*') || (cmd_args[1][0]=='?'))) {
    	
    		char rem[20];
			int q=0, k=0, p=0;
			char temp_fname[30];
			int fname_len=0;
			int cmd_len=0, rem_len=0;
			
			cmd_len=strlen(cmd_args[1]);
    		char *path=".";
			DIR *dirp=opendir(path);
			struct dirent *dp;
	
			for(q=1; q<cmd_len; q++)
			{
				rem[k++]=cmd_args[1][q];
			}
			rem_len=strlen(rem);
			
			if(cmd_args[1][0]=='*')
			{
				while((dp=readdir(dirp)) != NULL)
				{
					int ctr=0,m=0;
					strcpy(temp_fname, dp->d_name);
					fname_len=strlen(temp_fname);
					
				
					for(p=(fname_len-rem_len); p<fname_len; p++)
					{
						if(temp_fname[p]==rem[m++])
						{
							ctr++;
						}
					}
					if(ctr==rem_len)
					{
						printf("%s\n",dp->d_name);
					}
				}	
			}
			
			else if(cmd_args[1][0]=='?')
			{
				while((dp=readdir(dirp)) != NULL)
				{
					int ctr2=0, n=0;
					strcpy(temp_fname, dp->d_name);
					fname_len=strlen(temp_fname);
				
					for(p=(fname_len-rem_len); p<fname_len; p++)
					{
						if(temp_fname[p]==rem[n++])
						{
							ctr2++;
						}
					}
				
					if((ctr2==rem_len) && (fname_len==(rem_len+1)))
					{
						printf("%s\n",dp->d_name);
					}
				}	
			}
			
       		error_log("Wildcard matched!");
        }  

       	
    }  
    
    // NO PISTA COMMAND!
    error_log("PISTA COMMAND 0!");
    return 0;
}


int is_alias(char *key) {
    int i;
    for(i = 0 ; i < 5 ; i++)
        if(!strcmp(key, al[i].keys))
            return i;

    return -1;
}


int pista_delegate(char ***commands) {
    error_log("Entered pista delegate");

    pid_t chpid;
    int status = 0, i = 0, j, k, temp, bg = 0;
    int **pipes = NULL, *children = NULL;
    char **cmd_args, **redirs = NULL, **alias_cmd_args = NULL;
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

        temp = is_alias(cmd_args[0]);
        if(temp != -1) {
            char *tempaliasvalue = strdup(al[temp].values);
            alias_cmd_args = parse_cmd_args(tempaliasvalue);
            int i, aliaslen = 0;
            char *temp = alias_cmd_args[0];
            while(temp)
                temp = alias_cmd_args[++aliaslen];
            
            int cmd_args_len = 0;
            temp = cmd_args[0];
            while(temp)
                temp = cmd_args[++cmd_args_len];

            error_log("Alias len %d; cmdlen %d", aliaslen, cmd_args_len);

            alias_cmd_args = (char **)realloc(alias_cmd_args, sizeof(char *) * (aliaslen + cmd_args_len));
            
            temp = cmd_args[1];
            i = 1;
            while(temp) {
                alias_cmd_args[aliaslen++] = cmd_args[i];
                temp = cmd_args[i++];
            }
            alias_cmd_args[aliaslen] = NULL;
            cmd_args = alias_cmd_args;

            for(i = 0 ; i <= aliaslen ; i++)
                error_log("final %s", cmd_args[i]);
        }

        redirs = handle_redirections(cmd_args);
        if(!redirs) {
            error_log("Handle redirections error!");
            return 0;
        }
        else {
            infile = redirs[0];
            outfile = redirs[1];
            error_log("infile = %s\noutfile=%s", infile, outfile);
        }

        check_reallocs = realloc(pipes, sizeof(char *) * i);    // allocate pointer to store pipe
        if(check_reallocs) pipes = check_reallocs;
        else { perror("PIPES REALLOC ERROR"); _exit(errno); }


        pipes[i] = (int *)malloc(sizeof(int) * 2);      // allocate space for the actual pipe


        check_reallocs = realloc(children, sizeof(char *) * i);    // extend children pointer
        if(check_reallocs) children = check_reallocs;
        else { perror("PIPES REALLOC ERROR"); _exit(errno); }


        if( !(temp = pista_command(cmd_args)) ) {
            if(i == 0 && commands[i+1] == NULL) { // first and last
                error_log("first and last command");
                // handle input
                if(infile) {
                    instate = 1;
                    fdin = open(infile, O_RDONLY);   // open input file! Check for input redirection here!
                }
                else {
                    instate = 2;
                    fdin = dup(STDIN_FILENO);
                }

                // handle output
                if(outfile) {
                    outstate = 3;
                    fdout = open(outfile, O_CREAT | O_WRONLY | O_APPEND, 0664);   // open output file! OUTPUT redirection done here!
                    if(fdout < 0) perror("OP open error");
                }
                else {
                    outstate = 4;
                    fdout = dup(STDOUT_FILENO);
                }
            }
            else if(i == 0) {    // if first command
                error_log("First child!");
                
                // handle input
                if(infile) {
                    instate = 1;
                    fdin = open(infile, O_RDONLY);   // open input file! Check for input redirection here!
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
                    fdout = open(outfile, O_CREAT | O_TRUNC | O_WRONLY, 0664);   // open output file! OUTPUT redirection done here!
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

            bg = spawn_child_cmd(cmd_args, instate, fdin, outstate, fdout, pipes, children, i);
        }
        else 
            return temp;
        
        i++;    // loop variable! Important!
    }

    if(!bg) {
        error_log("Pista going to wait for children");
        int flag = 0;
        for(j = 0 ; j < i ; j++) {
            flag = 0;
            while( (chpid = waitpid(-1, &status, WUNTRACED | WNOHANG)) < 0) ;
            fflush(stdout); fflush(stderr);

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
            else {
                if(WIFEXITED(status))
                    error_log("%d exited with %d", chpid, WEXITSTATUS(status));
            }
        }
        error_log("Done waiting!");
    }
    
    error_log("Restoring default FDs");
    dup2(savedin, STDIN_FILENO);
    dup2(savedout, STDOUT_FILENO);
    close(savedin);
    close(savedout);

    free(pipes);
    free(children);

    return status;
}


int spawn_child_cmd(char **cmd_args, int instate, int fdin, int outstate, int fdout, int **pipes, int *children, int curr) {
    pid_t pid;
    
    int bg = 0;
    int i = 0;
    char *temp;
    while((temp = cmd_args[i+1]) != NULL)   // take temp to last but one cmd_args
        i++;
    if(!strcmp(cmd_args[i], "&")) {
        bg = 1;
        cmd_args[i] = NULL;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork error");
    }
    else if (pid == 0) {
        if(dup2(fdin, STDIN_FILENO) < 0) perror("dup2 of input failed");
        close(fdin);
        switch (instate) {
        case 1: case 2:
            break;

        case 3: case 4:
            close(pipes[curr - 1][WRITE_END]);
            break;
        }

        if(dup2(fdout, STDOUT_FILENO) < 0) perror("dup2 of output failed");
        close(fdout);
        switch (outstate) {
        case 1: case 5:
            close(pipes[curr][READ_END]);
            break;

        case 2: case 3: case 4:
            break;
        }

        int check_exec = execvp(cmd_args[0], cmd_args);

        error_log("CHILD Print post exec %s! THIS SHOULD NOT HAPPEN! errno = %d", cmd_args[0], errno);
        if (check_exec < 0)
        {
            #ifdef DEBUG_MODE
            perror("CHILD EXECVP ERROR");
            #endif
            printf(RED "%s : No such command or executable in $PATH!\n" RESET, cmd_args[0]);
        }

        exit(0);
    }
    else {
        error_log("pid = %d", pid);
        children[curr] = pid;
        if(timed_cmd) {
            error_log("setting up alarmer");
            if(fork() == 0) {
                char buf[100];
                strcpy(buf, "kill -14 ");
                if(cmd_args[0] == NULL)
                    strcat(buf, itoa(shell_pid));
                else
                    strcat(buf, itoa(pid));
                error_log("setting up alarmer %s", buf);
                sleep(timed_cmd);
                system(buf);
                _exit(0);
            }
            timed_cmd = 0;
        }
    }

    return bg;
}


char **handle_redirections(char **cmd_args) {
    char **ret = (char **)malloc(sizeof(char *) * 2);   // input file and output file names
    ret[0] = ret[1] = NULL;
    
    int i = 0, temp;
    while(cmd_args[i]) {
        if(cmd_args[i][0] == '<') {
            if(cmd_args[i+1]) {
                temp = open(cmd_args[i+1], O_RDONLY);
                if(temp < 0) {
                    perror(RED "Input file error");
                    fprintf(stderr, RESET);
                    return NULL;
                }
                close(temp);

                int len = strlen(cmd_args[i+1]);
                ret[0] = (char *)malloc(sizeof(char) * (len + 1 + 2));
                strcpy(ret[0], "./");
                strcat(ret[0], cmd_args[i+1]);
                
                int j;      // remove `< file` from cmd_args
                for(j = i ; cmd_args[j+1] != NULL ; j++)
                    cmd_args[j] = cmd_args[j+2];
            }
            else {
                fprintf(stderr, "Please specify input file!");
                return NULL;
            }
        }
        else if(cmd_args[i][0] == '>') {
            if(cmd_args[i+1]) {
                temp = open(cmd_args[i+1], O_RDWR);
                if(temp < 0) {
                    perror(RED "Output file error");
                    fprintf(stderr, RESET);
                    return NULL;
                }
                if(cmd_args[i][1] != '>')
                    ftruncate(temp, 0);
                close(temp);

                int temp = open(cmd_args[i+1], O_RDONLY);
                if(temp < 0) {
                    perror(RED "Output file error");
                    fprintf(stderr, RESET);
                    return NULL;
                }
                close(temp);

                int len = strlen(cmd_args[i+1]);
                ret[1] = (char *)malloc(sizeof(char) * (len + 1 + 2));
                strcpy(ret[1], "./");
                strcat(ret[1], cmd_args[i+1]);
                
                
                int j;      // remove `< file` from cmd_args
                for(j = i ; cmd_args[j+1] != NULL ; j++)
                    cmd_args[j] = cmd_args[j+2];
            }
            else {
                fprintf(stderr, "Please specify output file!");
                return NULL;
            }
        }

        i++;
        if(ret[0] && ret[1])
            break;
    }

    return ret;
}


void setup_signals() {
    struct sigaction sa;
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGTERM);
    sigaddset(&ss, SIGINT);
    sigaddset(&ss, SIGTSTP);
    sigprocmask(SIG_BLOCK, &ss, NULL);
    //signal(SIGTSTP, SIG_IGN);

    sa.sa_handler = alarm_handler;
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGALRM, &sa, NULL) < 0)
        perror("sigaction fail");
}

void alarm_handler(int opt) {
    printf("\aALARM!\n");
    fflush(stdout);
}