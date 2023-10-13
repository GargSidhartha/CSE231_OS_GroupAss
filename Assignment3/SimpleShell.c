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
#include<signal.h>
#include<sys/time.h>

#define COMLEN 1000
#define COMHISLEN 100 

#define clear() printf("\033[H\033[J")
#define SIZE 10
#define FLAG -1530494976

typedef struct {
    int *array;
    int head;
    int tail;
    int isEmpty;
    int isFull;
} CircularQueue;


int launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history);

int execute(CircularQueue * cq,char* command,char** command_history,int history_size, bool is_pipe);

int execute_pip(CircularQueue * cq,char* command, char** command_history, int history_size);

int shInterpreter(CircularQueue * cq,char* shFile, char** command_history, int history_size);


typedef struct command_info{
    int pid;
    long start_time;
    long end_time;
    double exec_time;
    char** command;
    int arg_count;
}command_info;

typedef struct process_info{
    int pid;
    bool status;
}process_info;

command_info* commands[COMHISLEN];
int com_counter = 0;
void initializeQueue(CircularQueue *cq, int size) {
    cq->array = (int *)malloc(size * sizeof(int));
    cq->head = cq->tail = cq->isEmpty = 0;
    cq->isFull = 0;
}

void enqueue(CircularQueue *cq, int ele) {
    if (cq->head == (cq->tail + 1) % SIZE && cq->isFull) {
        printf("\nerror: overflow\n");
    } else {
        cq->array[cq->tail] = ele;
        cq->isEmpty = 0;
        if (cq->tail == SIZE) {
            cq->tail = 0;
        } else {
            cq->tail += 1;
        }
        if (cq->head == cq->tail) {
            cq->isFull = 1;
        }
    }
}

int dequeue(CircularQueue *cq) {
    if (cq->head == cq->tail && cq->isEmpty == 1) {
        printf("\nerror: underflow\n");
        return FLAG;
    } else {
        int tmp = cq->array[cq->head];
        cq->head = (cq->head + 1) % SIZE;
        if (cq->head == cq->tail) {
            cq->isEmpty = 1;
        }
        return tmp;
    }
}

int qLength(CircularQueue *cq) {
    if (cq->head < cq->tail) {
        return cq->tail - cq->head;
    } else if (cq->head > cq->tail) {
        return SIZE - cq->head + cq->tail;
    } else if (cq->head == cq->tail && cq->isEmpty) {
        return 0;
    } else if (cq->head == cq->tail && cq->isFull) {
        return SIZE;
    } else {
        return FLAG;
    }
}

int front(CircularQueue *cq) {
    if (cq->isEmpty) {
        printf("queue is empty, ");
        return FLAG;
    }
    return cq->array[cq->head];
}

void printQueue(CircularQueue *cq) {
    if (cq->isEmpty) {
        printf("Queue is empty");
    } else if (cq->isEmpty == 0 && cq->head < cq->tail) {
        for (int i = cq->head; i < cq->tail; i++) {
            printf("%d ", cq->array[i]);
        }
    } else if (cq->isEmpty == 0 && cq->head >= cq->tail) {
        for (int i = cq->head; i < SIZE; i++) {
            printf("%d ", cq->array[i]);
        }
        for (int i = 0; i < cq->tail; i++) {
            printf("%d ", cq->array[i]);
        }
    }
}

void init_shell(int ncpu, int tslice){
    clear();
    printf("\n\n\n\n---------------------------------------------------");
    printf("\n\n\n\t**** ShellVetica ****");
    printf("\n\n\t-SakshsamSingh\n\tSidharthaGarg-\n\n");
    printf("\tNumber of CPUs: %d \n\tTSlice time: %d", ncpu, tslice);
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
        info->exec_time = ((double)(info->end_time - info->start_time)/(double)CLOCKS_PER_SEC) * 1000;
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
            printf("%lf \t",commands[i] -> exec_time);
            printf("\n");
            
        }
        exit(1);
    }
}

int execute(CircularQueue * cq,char* command,char** command_history,int history_size, bool is_pipe){
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
            printf("%lf \t",commands[i] -> exec_time);
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

int execute_pip(CircularQueue *cq, char* command, char** command_history, int history_size) {
    
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

int execute_ampersand(CircularQueue* cq,char* command, char** command_history, int history_size) {
    
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
        status = execute_pip(cq, sub_sentence, command_history, history_size);
        free(arg_ampersand[i]);
    }

    free(arg_ampersand);
    return status;
}

// int shInterpreter(char* shFile, char** command_history, int history_size){
//     FILE* file = fopen(shFile, "r");
//     if (file == NULL) {
//         printf("File not found\n");
//         return 1;
//     }

//     char* line = NULL;
//     size_t len = 0;
//     ssize_t read;

//     while ((read = getline(&line, &len, file)) != -1) {
//         if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
//         execute_ampersand(line, command_history, history_size);
//     }

//     fclose(file);
//     if (line) free(line);
//     return 1;

// }

int main(int argc, char** argv){

    CircularQueue *cq;
    initializeQueue(cq,1000);

    if(argc!=3){
        printf("Invalid number of arguments\n");
        return 1;
    }

    int ncpu = atoi(argv[1]);
    int tslice = atoi(argv[2]);

    init_shell(ncpu, tslice);

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
                status = execute_ampersand(cq,command,command_history,history_size);
                lseek(STDIN_FILENO, 0, SEEK_END);
            }
        }
    }

    while(status);

    return 0;
}
