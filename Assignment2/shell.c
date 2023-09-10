#include<stdio.h>
#include<string.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<stdbool.h>
#include<readline/readline.h>
#include<readline/history.h>
#define COMLEN 1000
#define COMHISLEN 100 

#define clear() printf("\033[H\033[J")

int launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history);

int execute(char* command,char** command_history,int history_size, bool is_pipe);

int execute_pip(char* command, char** command_history, int history_size);

int shInterpreter(char* shFile, char** command_history, int history_size);


typedef struct command_info{
    int pid;
    long start_time;
    long end_time;
    long exec_time;
    char** command;
    int arg_count;
}command_info;

command_info* commands[COMHISLEN];
int com_counter = 0;

void init_shell()
{
    clear();
    printf("\n\n\n\n---------------------------------------------------");
    printf("\n\n\n\t**** ShellVetica ****");
    printf("\n\n\t-SakshsamSingh\n\tSidharthaGarg-");
    printf("\n\n\n\n---------------------------------------------------");
    char* username = getenv("USER");
    if (username == NULL) {
        perror("getenv");
        exit(1);
    }
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    
}

int launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history){

    int fd[2];

    if(is_pipe) pipe(fd);

    int start_time = clock();
    int status = fork();
    

    if (status < 0) {
        printf("Forking failed\n");
    } else if (status == 0) {

        if(strcmp("history", args[0]) == 0){
            if (history_size != 0){
                for (int i = 0; i < history_size;i++ ){
                    printf("%s",command_history[i]);
                }
            }
            
        }
        else{
            if (is_pipe) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
            }

            if (execvp(args[0], args) < 0) {
                perror("Command not found");
            }
        }
        

        exit(1);
    } else {
        wait(NULL);
        
        if (is_pipe) {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
        }
    
        command_info* info = malloc(sizeof(command_info));
        // malloc NULL error handled
        if (info == NULL) {
            perror("malloc");
            exit(1);
        }

        info->pid = status;
        info->end_time = clock();
        info->start_time = start_time;
        info->exec_time = info->end_time - info->start_time;
        info->command = args;
        info->arg_count = arg_num;

        commands[com_counter] = info;
        com_counter++;

        return status;
    }
    return 1;
}

void cntrl_cHandler(int signum) {
    if(signum == SIGINT) {
        printf("\n");
        printf("\nCommand_History:\n");
        printf("PID\t\t Command\t\t StartTime\t ExecTime\n");
        for (int i = 0; i < com_counter; i++){
            printf("%d \t\t",commands[i] -> pid);
            for (int j = 0; j < commands[i] -> arg_count; j++){
                printf("%s ",commands[i] -> command[j]);
            }
            printf("\t\t%ld \t\t",commands[i] -> start_time);
            printf("%ld \t",commands[i] -> exec_time);
            printf("\n");
            
        }
        exit(1);
    }
}

int execute(char* command,char** command_history,int history_size, bool is_pipe){
    signal(SIGINT, cntrl_cHandler);

    int status = 1;
    
    //splitting the command
    char **args;
    int arg_num = 0;

    char *token = strtok(command, " ");
    args = malloc(sizeof(char*));
    if (args == NULL) {
        perror("malloc");
        exit(1);
    }
    while (token != NULL) {
        args[arg_num] = strdup(token);
        if (args[arg_num] == NULL) {
            perror("strdup");
            exit(1);
        }
        arg_num++;
        args = realloc(args, (arg_num + 1) * sizeof(char *));
        if (args == NULL) {
            perror("realloc");
            exit(1);
        }
        token = strtok(NULL, " ");
    }

    args[arg_num - 1][strcspn(args[arg_num - 1], "\n")] = '\0';

    //list down the builtin shell commands here to add support
    if(strcmp(args[0], "exit") == 0) {
        printf("\nCommand_History:\n");
        printf("PID\t\t Command\t\t StartTime\t ExecTime\n");
        for (int i = 0; i < com_counter; i++){
            printf("%d \t\t",commands[i] -> pid);
            for (int j = 0; j < commands[i] -> arg_count; j++){
                printf("%s ",commands[i] -> command[j]);
            }
            printf("\t\t%ld \t\t",commands[i] -> start_time);
            printf("%ld \t",commands[i] -> exec_time);
            printf("\n");
            
        }
        
        exit(0);
        return 0;
    }
        
    else{
        status = launch(args, arg_num, is_pipe, history_size, command_history);
    }
    
    return status;
}

int execute_pip(char* command, char** command_history, int history_size) {
    
    int status = 1;
    char** arg_pipe = NULL;
    int pipe_num = 0;

    char* token = strtok(command, "|");
    while (token != NULL) {
        arg_pipe = realloc(arg_pipe, (pipe_num + 1) * sizeof(char*));
        arg_pipe[pipe_num] = strdup(token);
        pipe_num++;
        token = strtok(NULL, "|");
    }

    for (int i = 0; i < pipe_num; i++) {
        char* sub_sentence = arg_pipe[i];
        // Trim leading and trailing spaces
        while (*sub_sentence == ' ') {
            sub_sentence++;
        }
        int len = strlen(sub_sentence);
        while (len > 0 && sub_sentence[len - 1] == ' ') {
            len--;
        }
        sub_sentence[len] = '\0';

        if (i < pipe_num - 1) {
            status = execute(sub_sentence, command_history, history_size, true);
        } else status = execute(sub_sentence, command_history, history_size, false);
        free(arg_pipe[i]);
    }

    free(arg_pipe);
    return status;
}

int execute_ampersand(char* command, char** command_history, int history_size) {
    
    int status = 1;
    char** arg_ampersand = NULL;
    int ampersand_num = 0;

    char* token = strtok(command, "&");
    while (token != NULL) {
        arg_ampersand = realloc(arg_ampersand, (ampersand_num + 1) * sizeof(char*));
        arg_ampersand[ampersand_num] = strdup(token);
        ampersand_num++;
        token = strtok(NULL, "&");
    }

    for (int i = 0; i < ampersand_num; i++) {
        char* sub_sentence = arg_ampersand[i];
        
        while (*sub_sentence == ' ') {
            sub_sentence++;
        }
        int len = strlen(sub_sentence);
        while (len > 0 && sub_sentence[len - 1] == ' ') {
            len--;
        }
        sub_sentence[len] = '\0';
        status = execute_pip(sub_sentence, command_history, history_size);
        free(arg_ampersand[i]);
    }

    free(arg_ampersand);
    return status;
}


int shInterpreter(char* shFile, char** command_history, int history_size){
    FILE* file = fopen(shFile, "r");
    if (file == NULL) {
        printf("File not found\n");
        return 1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        execute_ampersand(line, command_history, history_size);
    }

    fclose(file);
    if (line) free(line);
    return 1;

}

int main(){
    init_shell();

    int status;
    char** command_history = NULL;
    int history_size = 0;

    do{
        char* user = getenv("USER");
        
        printf("\n%s@shellvetica:~$ > ",user);
    
        char command[COMLEN];

        
        if(fgets(command,COMLEN,stdin)==NULL){

            if (feof(stdin)) {                      // end of input reached
                status = 1;
                
                
                if (freopen("/dev/tty", "r", stdin) == NULL) {
                    perror("freopen");
                    exit(1);
                }

            }
            else if (ferror(stdin)) {               // some other error occurred
                status = 1;
                command[strcspn(command, "\n")] = '\0';
                printf("Some error occurred. Enter again.\n");
            }
        }

        else{
            
            command_history = realloc(command_history, (history_size + 1) * sizeof(char *));
            command_history[history_size] = strdup(command);
            history_size++;

            if(command[0]=='.' && command[1]=='/' && command[strlen(command)-1]=='h' && command[strlen(command)-2]=='s'){
                status = shInterpreter(command, command_history, history_size);
            }
            else{
                status = execute_ampersand(command,command_history,history_size);
                lseek(STDIN_FILENO, 0, SEEK_END);
            }

        }
    }
    while(status);

    return 0;
}
