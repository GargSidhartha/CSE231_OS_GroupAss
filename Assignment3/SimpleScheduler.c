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
#include<semaphore.h>
#include<sys/mman.h>
#include <fcntl.h>

#define COMLEN 1000
#define COMHISLEN 100 
#define JOBMAX 100

#define clear() printf("\033[H\033[J")

int shm_fd;
int com_fd;
int flag = 1;
int flag_process = 0;
// int ans = 0;
// volatile sig_atomic_t flag = 0;


typedef struct command_info{
    int pid;
    long start_time;
    long end_time;
    long wait_time;
    long exec_time;
    char** command;
    int arg_count;
}command_info;

typedef struct{
    int pid;
    bool state; // 0 for ready, 1 for running
}process;

typedef struct {
    process queue[JOBMAX];
    int front;
    int rear;
    int isEmpty;
    sem_t mutex;
}Queue;

//function to enqueue a process
void enqueue(Queue* q, process p){
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear = (q->rear + 1) % JOBMAX;
    q->queue[q->rear] = p;
    q->isEmpty = 0;
}

//function to dequeue a process
process dequeue(Queue* q){
    process p = q->queue[q->front];
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
        q->isEmpty = 1;
    } else {
        q->front = (q->front + 1) % JOBMAX;
    }
    return p;
}

typedef struct {
    Queue q1;
    Queue q2;
    Queue q3;
    Queue q4;
}PriorityQueues;

PriorityQueues* priorityQueues;

int launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history);

int submit_launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history);

int execute(char* command,char** command_history,int history_size, bool is_pipe);

int execute_pip(char* command, char** command_history, int history_size);

int shInterpreter(char* shFile, char** command_history, int history_size);

PriorityQueues* setup_pq();

void sigchld_handler(int signo);


void cleanup();

void cleanup_and_exit();

typedef struct{
    command_info arr[COMHISLEN];
    int com_counter;
    sem_t mutex;
}Commands;

Commands* commands;

Commands* setup_coms();


void init_shell(int ncpu, int tslice){
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
    printf("\n\n\nUSER is: @%s \t", username);
    printf("ncpu is: %d tslice: %d", ncpu, tslice);
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
    
        // command_info* info = malloc(sizeof(command_info));
        // malloc NULL error handled
        // if (info == NULL) {
        //     perror("malloc");
        //     exit(1);
        // }
        command_info info;
        
        sem_wait(&(commands->mutex));
        info.pid = status;
        info.end_time = clock();
        info.start_time = start_time;
        info.exec_time = info.end_time - info.start_time;
        info.command = args;
        info.arg_count = arg_num;
        info.wait_time = 0;

        commands->arr[commands->com_counter] = info;
        commands->com_counter++;
        sem_post(&(commands->mutex));

        return status;
    }
    return 1;
}

int submit_launch(char** args, int arg_num, bool is_pipe, int history_size, char** command_history){
    int start_time = clock();
    int status = fork();
    
    if (status < 0) {
        printf("Forking failed\n");
    } else if (status == 0) {
        if (execvp(args[1], NULL) < 0) {
                    perror("Command not found");
                    
            }
    } else {
        // waitpid(status,NULL,0);
        signal(SIGCHLD,sigchld_handler);
        int kill_result = kill(status, SIGSTOP);

        if (arg_num == 2){
            
            
            sem_wait(&(priorityQueues->q1.mutex));
            
            if (priorityQueues->q1.front == -1) {
                priorityQueues->q1.front = 0;
            }
            priorityQueues->q1.rear = (priorityQueues->q1.rear + 1) % JOBMAX;
            priorityQueues->q1.queue[priorityQueues->q1.rear].pid = status;
            priorityQueues->q1.queue[priorityQueues->q1.rear].state = 0;
            priorityQueues->q1.isEmpty = 0;
            sem_post(&(priorityQueues->q1.mutex));

        }
        else if(arg_num == 3){
            int priority = atoi(args[2]);
            if (priority == 1){
                // add to priority queue 1 in shared memory
                sem_wait(&(priorityQueues->q1.mutex));
                if (priorityQueues->q1.front == -1) {
                    priorityQueues->q1.front = 0;
                }
                
                priorityQueues->q1.rear = (priorityQueues->q1.rear + 1) % JOBMAX;
                priorityQueues->q1.queue[priorityQueues->q1.rear].pid = status;
                priorityQueues->q1.queue[priorityQueues->q1.rear].state = 0;
                priorityQueues->q1.isEmpty = 0;
                sem_post(&(priorityQueues->q1.mutex));
            }
            else if(priority == 2){
                // add to priority queue 2 in shared memory
                if (sem_wait(&(priorityQueues->q2.mutex)) != 0){
                    perror("sem_wait");
                }
                if (priorityQueues->q2.front == -1) {
                    priorityQueues->q2.front = 0;
                }
                priorityQueues->q2.rear = (priorityQueues->q2.rear + 1) % JOBMAX;
                priorityQueues->q2.queue[priorityQueues->q2.rear].pid = status;
                priorityQueues->q2.queue[priorityQueues->q2.rear].state = 0;
                priorityQueues->q2.isEmpty = 0;
                sem_post(&(priorityQueues->q2.mutex));
                

            }
            else if(priority == 3){
                // add to priority queue 3 in shared memory
                sem_wait(&(priorityQueues->q3.mutex));
                if (priorityQueues->q3.front == -1) {
                    priorityQueues->q3.front = 0;
                }
                priorityQueues->q3.rear = (priorityQueues->q3.rear + 1) % JOBMAX;
                priorityQueues->q3.queue[priorityQueues->q3.rear].pid = status;
                priorityQueues->q3.queue[priorityQueues->q3.rear].state = 0;
                priorityQueues->q3.isEmpty = 0;
                sem_post(&(priorityQueues->q3.mutex));

                
            }
            else if(priority == 4){
                // add to priority queue 4 in shared memory
                sem_wait(&(priorityQueues->q4.mutex));
                if (priorityQueues->q4.front == -1) {
                    priorityQueues->q4.front = 0;
                }
                priorityQueues->q4.rear = (priorityQueues->q4.rear + 1) % JOBMAX;
                priorityQueues->q4.queue[priorityQueues->q4.rear].pid = status;
                priorityQueues->q4.queue[priorityQueues->q4.rear].state = 0;
                priorityQueues->q4.isEmpty = 0;
                sem_post(&(priorityQueues->q4.mutex));

                
            }
            else{
                printf("Invalid priority\n");
            }
        }
        
        // command_info* info = malloc(sizeof(command_info));
        // // malloc NULL error handled
        // if (info == NULL) {
        //     perror("malloc");
        //     exit(1);
        // }
        command_info info;

        sem_wait(&(commands->mutex));
        info.pid = status;
        info.end_time = clock();
        info.start_time = start_time;
        info.exec_time = info.end_time - info.start_time;
        info.command = args;
        info.arg_count = arg_num;
        info.wait_time = 0;

        commands->arr[commands->com_counter] = info;
        commands->com_counter++;
        sem_post(&(commands->mutex));

        return status;
    }
    return 1;
}

void signal_Handler(int signum) {
    if(signum == SIGINT) {
        printf("\n");
        printf("\nCommand_History:\n");
        printf("PID\t\t Command\t\t StartTime\t WaitTime\t ExecTime\n");
        sem_wait(&(commands->mutex));
        for (int i = 0; i < commands->com_counter; i++){
            printf("%d \t\t",commands->arr[i].pid);
            for (int j = 0; j < commands->arr[i].arg_count; j++){
                printf("%s ",commands->arr[i].command[j]);
            }
            printf("\t\t%ld \t\t",commands->arr[i].start_time);
            printf("%ld \t",commands->arr[i].wait_time);
            printf("%ld \t",commands->arr[i].exec_time);
            printf("\n");
            
        }
        sem_post(&(commands->mutex));
        cleanup_and_exit();
        exit(1);
    }

    
}

void sigchld_handler(int signo) {
    if (flag) {
        flag_process = 1;
        // Handle termination logic here
    }
}

void sigstop_handler(int signo) {
    // Toggle the flag when SIGSTOP is received
    flag = !flag;
    printf("SIGSTOP received - Toggle flag to %d\n", flag);
}

int execute(char* command,char** command_history,int history_size, bool is_pipe){
    signal(SIGINT, signal_Handler);

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
        printf("PID\t\t Command");
        sem_wait(&(commands->mutex));
        for (int i = 0; i < commands->com_counter; i++){
            printf("%d \t\t",commands->arr[i].pid);
            for (int j = 0; j < commands->arr[i].arg_count; j++){
                printf("%s ",commands->arr[i].command[j]);
            }
            printf("\n");
            
        }
        sem_post(&(commands->mutex));
        
        exit(0);
        return 0;
    }

    //submit launch
    else if(strcmp(args[0], "submit") == 0){
            int status = submit_launch(args, arg_num, false, history_size, command_history);
            return status;
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

int shell(int ncpu, int tslice){
    
    init_shell(ncpu, tslice);

    int status;
    char** command_history = NULL;
    int history_size = 0;
    

    do{
        // signal(SIGQUIT, signal_Handler);
        // if (flag == 1){
        //     break;
        // }
        // printf("%d",ans);
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
        // printf("DO YOU WANT TO START SCHEDULER?\n");
        // scanf("%d",&ans);
        // if (ans == 1){
        //     break;
        // }

    }
    while(status);

    return 0;
}




//----------------------------scheduler starts here---------------------------------





PriorityQueues* setup_pq() {
    shm_fd = shm_open("scheduler", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(PriorityQueues));

    PriorityQueues* pq = mmap(NULL, sizeof(PriorityQueues), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (pq == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pq->q1.front = -1;
    pq->q1.rear = -1;
    pq->q2.front = -1;
    pq->q2.rear = -1;
    pq->q3.front = -1;
    pq->q3.rear = -1;
    pq->q4.front = -1;
    pq->q4.rear = -1;

    pq->q1.isEmpty = 1;
    pq->q2.isEmpty = 1;
    pq->q3.isEmpty = 1;
    pq->q4.isEmpty = 1;

    for(int i = 0; i < JOBMAX; i++){
        pq->q1.queue[i].pid = 0;
        pq->q1.queue[i].state = 0;
        pq->q2.queue[i].pid = 0;
        pq->q2.queue[i].state = 0;
        pq->q3.queue[i].pid = 0;
        pq->q3.queue[i].state = 0;
        pq->q4.queue[i].pid = 0;
        pq->q4.queue[i].state = 0;
    }

    sem_init(&(pq->q1.mutex), 1, 1);
    sem_init(&(pq->q2.mutex), 1, 1);
    sem_init(&(pq->q3.mutex), 1, 1);
    sem_init(&(pq->q4.mutex), 1, 1);

    return pq;
}

Commands* setup_coms(){
    com_fd = shm_open("commands", O_CREAT | O_RDWR, 0666);
    if (com_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(com_fd, sizeof(Commands));

    Commands* coms = mmap(NULL, sizeof(Commands), PROT_READ | PROT_WRITE, MAP_SHARED, com_fd, 0);
    if (coms == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    coms->com_counter = 0;

    sem_init(&(coms->mutex), 1, 1);

    return coms;
}

void cleanup(){
    munmap(priorityQueues, sizeof(PriorityQueues));
    munmap(commands, sizeof(Commands));
    close(shm_fd);
    close(com_fd);
    shm_unlink("scheduler");
    shm_unlink("commands");
    sem_destroy(&(priorityQueues->q1.mutex));
    sem_destroy(&(priorityQueues->q2.mutex));
    sem_destroy(&(priorityQueues->q3.mutex));
    sem_destroy(&(priorityQueues->q4.mutex));
    sem_destroy(&(commands->mutex));
}

void cleanup_and_exit(){
    munmap(priorityQueues, sizeof(PriorityQueues));
    munmap(commands, sizeof(Commands));
    close(shm_fd);
    close(com_fd);
    exit(0);
}

int scheduler(int ncpu, int tslice){
    while(1){
        while(priorityQueues -> q1.isEmpty == 0 || priorityQueues -> q2.isEmpty == 0 || priorityQueues -> q3.isEmpty == 0 || priorityQueues -> q4.isEmpty == 0) {

        //implement round robin on each queue seperately
        for(int i = 0; i < ncpu; i++){
            

            if(priorityQueues -> q4.isEmpty == 0){
                sem_wait(&(priorityQueues->q4.mutex));
                process p = dequeue(&(priorityQueues->q4));
                sem_post(&(priorityQueues->q4.mutex));

                //sigcont the process
                p.state = 1;
                int kill_result = kill(p.pid, SIGCONT);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                usleep(tslice*1000);

                sem_wait(&(commands->mutex));
                for (int i = 0; i < commands->com_counter; i++){
                    if (commands->arr[i].pid == p.pid){
                        commands->arr[i].wait_time += tslice;
                    }
                }
                sem_post(&(commands->mutex));
                
        

                if (flag_process == 1){
                    flag_process = 0;
                    break;
                } 

                //sigstop the process
                kill_result = kill(p.pid, SIGSTOP);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                p.state = 0;

                //enqueue the process at the end of the queue
                if(flag_process==0){
                    sem_wait(&(priorityQueues->q4.mutex));
                    enqueue(&(priorityQueues->q4), p);
                    sem_post(&(priorityQueues->q4.mutex));
                }
                
                continue;
            }
        
            if(priorityQueues -> q3.isEmpty == 0){
                sem_wait(&(priorityQueues->q3.mutex));
                process p = dequeue(&(priorityQueues->q3));
                sem_post(&(priorityQueues->q3.mutex));

                //sigcont the process
                p.state = 1;
                int kill_result = kill(p.pid, SIGCONT);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                usleep(tslice*1000);

                sem_wait(&(commands->mutex));
                for (int i = 0; i < commands->com_counter; i++){
                    if (commands->arr[i].pid == p.pid){
                        commands->arr[i].wait_time += tslice;
                    }
                }
                sem_post(&(commands->mutex));
                if(flag_process == 1){
                    flag_process = 0;
                    break;
                }

                //sigstop the process
                kill_result = kill(p.pid, SIGSTOP);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                p.state = 0;

                //enqueue the process at the end of the queue
                if(flag_process==0){
                    sem_wait(&(priorityQueues->q3.mutex));
                    enqueue(&(priorityQueues->q3), p);
                    sem_post(&(priorityQueues->q3.mutex));
                }
                continue;
            }

            if(priorityQueues -> q2.isEmpty == 0){
                sem_wait(&(priorityQueues->q2.mutex));
                process p = dequeue(&(priorityQueues->q2));
                sem_post(&(priorityQueues->q2.mutex));

                //sigcont the process
                p.state = 1;
                int kill_result = kill(p.pid, SIGCONT);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                usleep(tslice*1000);

                sem_wait(&(commands->mutex));
                for (int i = 0; i < commands->com_counter; i++){
                    if (commands->arr[i].pid == p.pid){
                        commands->arr[i].wait_time += tslice;
                    }
                }
                sem_post(&(commands->mutex));

                if(flag_process == 1){
                    flag_process = 0;
                    break;
                }
                //sigstop the process
                kill_result = kill(p.pid, SIGSTOP);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                p.state = 0;

                //enqueue the process at the end of the queue
                if(flag_process==0){
                    sem_wait(&(priorityQueues->q2.mutex));
                    enqueue(&(priorityQueues->q2), p);
                    sem_post(&(priorityQueues->q2.mutex));
                }
                continue;
            }

            if(priorityQueues -> q1.isEmpty == 0){
                sem_wait(&(priorityQueues->q1.mutex));
                process p = dequeue(&(priorityQueues->q1));
                sem_post(&(priorityQueues->q1.mutex));

                //sigcont the process
                p.state = 1;
                int kill_result = kill(p.pid, SIGCONT);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                usleep(tslice*1000);

                sem_wait(&(commands->mutex));
                for (int i = 0; i < commands->com_counter; i++){
                    if (commands->arr[i].pid == p.pid){
                        commands->arr[i].wait_time += tslice;
                    }
                }
                sem_post(&(commands->mutex));

                if(flag_process == 1){
                    flag_process = 0;
                    break;
                }

                //sigstop the process
                kill_result = kill(p.pid, SIGSTOP);
                if (kill_result == -1) {
                    perror("kill");
                    exit(1);
                }
                p.state = 0;

                //enqueue the process at the end of the queue
                if(flag_process==0){
                    sem_wait(&(priorityQueues->q1.mutex));
                    enqueue(&(priorityQueues->q1), p);
                    sem_post(&(priorityQueues->q1.mutex));
                }
                continue;
            } 

        }
    }
    

    }

    return 0;
}


int main(int argc, char** argv){
    if(argc != 3){
        printf("Incorrect no of arguments\n");
        return 1;
    }
    int ncpu = atoi(argv[1]);
    int tslice = atoi(argv[2]);

    priorityQueues = setup_pq();
    commands = setup_coms();
    
    int status;
    pid_t schedule = fork();
    if(schedule < 0){
        printf("Forking failed\n");
    }
    else if(schedule == 0){
        scheduler(ncpu,tslice);
    }
    else{
        shell(ncpu, tslice);
        waitpid(schedule, NULL, 0);
    }

    cleanup();

    return 0;
}