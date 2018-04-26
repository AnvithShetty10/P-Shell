/*
This file handles everything related to command string processing, such as, extracting commands and their arguments.
*/

/* -------------------- Includes -------------------- */
#include "cmd_processor.h"
#include "pista_main.h"

/* -------------------- Statics and Globals -------------------- */

/* Error logger function for THIS file ONLY. */
#ifndef DEBUG_MODE
#define error_log(fmt, ...) ;
#endif
#ifdef DEBUG_MODE
static void error_log(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    dprintf(STDERR_FILENO, "\n");
    dprintf(STDERR_FILENO, "CMD PROCESSOR : ");
    vdprintf(STDERR_FILENO, fmt, args);
    dprintf(STDERR_FILENO, "\n");
    fflush(stderr);

    va_end(args);
}
#endif


/* -------------------- Function Definitions -------------------- */
int curr=0;
int command_index=0;
extern int hist_cmd_count;
extern int reverse_flag;
char hist_command[100][50];
int switch_arrow = 0;

char *processor(char *buf) {
    error_log("processor!");
    error_log("buf is %p", buf);
    set_terminal();
    int blen = 0;
    char fc[4];
    int stop_right = 0;
    char tbuf[1000] = {0};
    fc[0] = getPressedKey();
    error_log("got fc0 as %c %d", fc[0], fc[0]);
    while(fc[0] != '\n' && fc[0] != '\r') {
        error_log("got %d", fc[0]);
        if (fc[0]==127)
        {   
            if(blen>0){
                blen--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if(fc[0] == 9) {   // handle tabs
            error_log("tabs");
            char searchFor[500];
            int temp_blen = blen - 1;

            if(temp_blen > 0) {
                error_log("here tab");

            while(buf[temp_blen] != ' ' && temp_blen > 0)
                temp_blen--;

            error_log("temp blen %d %d", temp_blen, blen);
            
            strcpy(searchFor, "ls | grep ");
            buf[blen] = 0;
            buf[blen + 1] = 0;
            if(temp_blen)
                temp_blen++;
            
            strcat(searchFor, buf + temp_blen);
            
            error_log("tab searching for %s", searchFor);

            FILE *op = popen(searchFor, "r");

            char sop[100];
            if(fgets(sop, 100, op) != NULL) {
                error_log("tab got %s", sop);
                int i;
                restore_terminal();
                for(i = 0 ; sop[i] != '\0' && sop[i] == buf[temp_blen] ; i++, temp_blen++);

                for(; sop[i] != '\0' && sop[i] != '\n' ; i++) {
                    buf[blen++] = sop[i];
                    printf("%c", sop[i]);
                }
                fflush(stdout);
                set_terminal();
            }
            else {
                if(blen>0){
                    blen--;
                    printf("\b \b");
                    fflush(stdout);
                }
            }
            }

        }
        else if(fc[0] == IS_CTRL_KEY('c')) {
            ; //do nothing
        }
        else if(fc[0] == IS_CTRL_KEY('z')) {
            ; //do nothing
        }
        else if (fc[0] == 27) { // if the first value is esc
            fc[1] = getPressedKey();
            error_log("got 1 %d", fc[1]);
            if(fc[1] == 91){           //Can use '[' also
                fc[2] = getPressedKey();
                error_log("got 2 %d", fc[2]);
                switch(fc[2]) { 
                    case 'A':
                        if(curr == 0) {
                            strcpy(tbuf, buf);
                            error_log("copied %s to tbuf", buf);
                        }
                        if(curr!=hist_cmd_count)
                            {
                            clear_prompt_up();
                            UpArrow();
                            strcpy(buf,hist_command[curr-1]);
                            blen = strlen(buf);
                            fflush(stdout);
                            }
                        break;
                    case 'B':
                        error_log("case B down array with curr %d", curr);
                        if(curr!=0){
                            clear_prompt_down();
                            DownArrow();
                            strcpy(buf,hist_command[curr]);
                            blen = strlen(buf);
                            fflush(stdout);
                        }
                        error_log("down curr %d", curr);
                        if(curr<=0){
                            clear_prompt_down();
                            strcpy(buf, tbuf);
                            blen = strlen(buf);
                            error_log("copied %s to buf", tbuf);
                            printf("%s", buf);
                            fflush(stdout);
                        }
                        break;
                    case 'C':
                        error_log("case c blen = %d", blen);
                        if(!stop_right) {
                            if(buf[blen] == 0) {
                                stop_right = 1;
                            }
                            else {
                                blen++;
                                restore_terminal();
                                printf("\033[1C");
                                fflush(stdout);
                                set_terminal();
                            }
                        }
                        break;
                    case 'D':
                        error_log("case d blen = %d", blen);
                        if(blen > 0){
                            blen--;
                            error_log("blen = %d", blen);
                            restore_terminal();
                            printf("\033[1D");
                            fflush(stdout);
                            set_terminal();
                            stop_right = 0;
                        }
                        break;
                    }
                }
            }
        else {
            error_log("processor ELSE blen = %d", blen);
            buf[blen] = fc[0];
            blen += 1;
            buf[blen] = 0;
            curr=0;
            restore_terminal();
            printf("%c", fc[0]);
            fflush(stdout);
            set_terminal();
        }
        fc[0] = getPressedKey();
        error_log("got fc0 as %c %d", fc[0], fc[0]);
    }

    buf[blen] = 0;
    curr = 0;
    restore_terminal();
    //write(STDOUT_FILENO, '\r', sizeof(char));
    printf("\n");
    error_log("buf is %s", buf);
    return buf;
}

struct termios oldt, newt;
int set_terminal(){
    error_log("set terminal!");
    // get current terminal properties
    if (tcgetattr(0 , &(oldt)) != 0) {
        perror("Couldn't get terminal properties!");
        exit(0);
    }
    /*now the settings will be copied*/
    newt = oldt;
   /* newt.c_iflag &= ~(IXON | ICRNL); 
    // disable ctrl-S and ctrl-Q (software data flow control), ctrl-m, 

    newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
    // disable echoing, canonical mode (so we can read byte-by-byte), ctrl-v and ctrl-o, ctrl-C

    newt.c_oflag &= ~(OPOST);
    // disable output processing

    newt.c_oflag &= ~(BRKINT | INPCK | ISTRIP | CS8);
    // for compatibility apparently*/
    
    //cfmakeraw(&newt);

    newt.c_lflag &= ~ICANON; /* disable buffered i/o */
    //newt.c_lflag &= ~ECHO; /* set echo mode */
    newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
    /* use these new terminal i/o settings now */
    if (tcsetattr(0, TCSANOW, &newt) != 0) {
        perror("Couldn't set terminal properties!");
        exit(0);
    }

/*    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt) != 0) {
        perror("Couldn't set terminal properties!");
        exit(0);
    }
*/
  return 0;

}

int restore_terminal(){
    error_log("restore terminal!");
    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}

char getPressedKey() {
    char c;

    if(read(STDIN_FILENO, &c, 1) < 1) {
        perror("Read error");
        exit(0);
    }

    return c;
}

void readfileinreverse(FILE *fp)
{
    error_log("readfile in reverse!");
    int i, size, start, loop, counter;
    int k,j,space;
    char *buffer;
    char line[256];
    start = 0;
    error_log("set start to 0");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    char hist_cmd[256];

    buffer = malloc((size+1) * sizeof(char));

    for (i=0; i< size; i++)
    {
        fseek(fp, size-1-i, SEEK_SET);
        buffer[i] = fgetc(fp);

        if(buffer[i] == 10)
        {
           if(i != 0)
           {
            counter = 0;        
            for(loop = i; loop > start; loop--)
            {
                if((counter == 0) && (buffer[loop] == 10))
                {
                    continue;
                }               
                line[counter] = buffer[loop];
                counter++;
            }
            line[counter] = 0;
            start = i;

            k=0;
            space=0;
            while(line[k]!='\0'){
                if(line[k] == ' ')
                    space++;
                if(space == 7)
                    break;
                else
                    k++;
            }
            j=0;
            k+=2;
            strcpy(hist_cmd,"");
            while(line[k]!='\0'){
                hist_cmd[j] = line[k];
                k++;
                j++;
            }
                hist_cmd[j++]='\0';

            if(command_index >= 100)
                break;
            strcpy(hist_command[command_index++],hist_cmd);
            error_log("loaded %s to history at %d", hist_cmd, command_index);
            
           }
        }
    }

    //For the first command
    if(i > start)
        {    
            counter = 0;
            for(loop = i; loop > start; loop--)
            {       
                if((counter == 0) && ((buffer[loop] == 10) || (buffer[loop] == 0)))
                {
                    continue;
                }               
                line[counter] = buffer[loop];
                counter++;
            }
            line[counter] = 0;
        }

        k=0;
        space=0;
        while(line[k]!='\0'){
            if(line[k] == ' ')
                space++;
            if(space == 7)
                break;
            else
                k++;
        }

        j=0;
        k+=2;
        strcpy(hist_cmd,"");
        while(line[k]!='\0'){
            hist_cmd[j] = line[k];
            k++;
            j++;
        }

        hist_cmd[j++]='\0';
        strcpy(hist_command[command_index++],hist_cmd);
        reverse_flag=0;
    return;
}

void clear_prompt_up(){
    int i;
    fflush(stdout);
    for(i=0;i<100;i++)
        {
            printf("\b \b");
        }
    fflush(stdout);
    print_prompt();
}

void clear_prompt_down(){
    int i;
    fflush(stdout);
    for(i=0;i<100;i++)
        {
            printf("\b \b");
        }
    fflush(stdout);
    print_prompt();
}

int UpArrow(){
    error_log("up arrow, rev flag = %d", reverse_flag);
    FILE * fd=fopen(histPath,"r");
    if (reverse_flag==1)
    {
        readfileinreverse(fd);
    }
    printf("%s",hist_command[curr++] );
    fclose(fd);
    if(switch_arrow==0)
        switch_arrow=1;
    //print_prompt();
    error_log("leaving up arrow");
    return 0;
}

int DownArrow(){
    error_log("down arrow");
    FILE * fd=fopen(histPath,"r");
    if(switch_arrow==1){
        curr-=1;
        switch_arrow=0;
    }
    printf("%s",hist_command[--curr] );
    fclose(fd);
    //print_prompt();
    error_log("leaving down arrow");
    return 0;
}

char **parse_cmd_args(char *full_cmd) {
    char **ret = NULL;

    int count = 0, len = strlen(full_cmd) - 1;
    char *temp = full_cmd;
    error_log("len = %d; temp[len] = %c; *temp = %c", len, temp[len], *temp);
    while(temp[len] == ' ')
        temp[len--] = 0;
    while(*temp == ' ')             // strip spaces
        temp++;

    void *check = NULL;
    do {
        check = realloc(ret, sizeof(char *) * (count + 1));
        if(check)
            ret = check;
        else {
            perror("FAILED TO REALLOC TABLE OF CMD AND ARGS"); 
            return NULL;
        }

        ret[count++] = temp;

        // Process env variables
        if(ret[count-1][0] == '$') {
            ret[count-1] = getenv(ret[count-1] + 1);
        }

        tokenize(&temp);
        error_log("After tokenize %s", temp);

        /*// If string is given in quotes, remove the quotes
        int index, len = strlen(ret[count-1]), jindex;
        for(index = 0 ; index < len ; index++) {
            if(ret[count-1][index] == '"')      // Check for quotes "
            {
                for(jindex = index; jindex < (len - 1) ; jindex++)
                    ret[count-1][jindex] = ret[count-1][jindex + 1];      // move chars back, removing the quotes
                len--;      // reduce length
            }
        }

        ret[count-1][len] = 0;      // set new nul char to terminate string*/

    }while(temp);

    if(temp && strlen(temp) > 0) {
        check = realloc(ret, sizeof(char *) * (++count));
        if(check)
            ret = check;
        else {
            perror("FAILED TO REALLOC TABLE OF CMD AND ARGS 2");
            return NULL;
        }
    }

        ret[count - 1] = temp;

    check = realloc(ret, sizeof(char *) * (++count));
    if(check)
        ret = check;
    else {
        perror("FAILED TO REALLOC TABLE OF CMD AND ARGS 2");
        return NULL;
    }

    ret[count - 1] = NULL;  // add final NULL


    // Printing all arguments for debugging
    #ifdef DEBUG_MODE
    int i = 0; temp = *(ret + i); while(temp) { error_log("CMDARGS %s", temp); temp = *(ret + ++i); }
    #endif
    error_log("Done parsing command and args!");
    return ret;
}


char ***parse_commands(char *full_cmd) {
    error_log("Entered parse_commands");
    char ***ret = NULL;

    int count = 0;
    char *temp = full_cmd, *temp_cmd = NULL;
    void *check = NULL;
    do {
        check = realloc(ret, sizeof(char **) * (count + 1));
        if(check)
            ret = check;
        else {
            perror("FAILED TO REALLOC TABLE OF COMMANDS");
            return NULL;
        }

        error_log("Before strsep on pipe; temp = %s", temp);
        temp_cmd = strsep(&temp, "|");
        error_log("strsep on pipe: temp_cmd = %s; temp = %s", temp_cmd, temp);

        ret[count++] = parse_cmd_args(temp_cmd);
    }while(temp);

    check = realloc(ret, sizeof(char **) * (count + 1));
    if(check)
        ret = check;
    else {
        perror("FAILED TO REALLOC TABLE OF COMMANDS");
        return NULL;
    }
    ret[count++] = NULL;

    #ifdef DEBUG_MODE
    error_log("Printing all commands for debugging"); int i = 0; char **temp_; temp_ = *(ret + i); while(temp_) { error_log("CMD %s", temp_[0]); temp_ = *(ret + ++i); }
    #endif

    error_log("Done parsing command!");
    return ret;
}


void tokenize(char **s_) {
    char *s = *s_;
    int i;
    int looped = 0;

    if(s[0] == 0) {
        *s_ = NULL;
        error_log("5token : %s", *s_);
        return;
    }

    for(i = 0 ; s[i] ; i++) {
        error_log("tokchar %c %d", s[i], s[i]);
        looped = 1;
        switch(s[i]) {
            case '"':
                error_log("quote at %d", i);
                for(i = i + 1 ; s[i] != '"' && s[i] ; i++);
                error_log("equote at %d", i);
                if(s[i]) {
                    s[i+1] = 0;
                    *s_ = s + i + 2;
                    return;
                }
                else {
                    *s_ = NULL;
                    return;
                }
                error_log("3token : %s", *s_);
            break;

            case ' ': case '\t':
                s[i] = 0;
                *s_ = s + i + 1;
                error_log("1token : %s", *s_);
                return;
            
            case 0:
                break;
        }
    }

    if(looped)
        *s_ = s + i;
    else
        *s_ = NULL;
    error_log("2token : %s", *s_);
}