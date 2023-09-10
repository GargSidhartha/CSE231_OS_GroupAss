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
