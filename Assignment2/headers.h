#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<time.h>
#include<stdbool.h>
#define COMLEN 1000
#define COMHISLEN 100 

#define clear() printf("\033[H\033[J")