#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){

    char* command[] = {"grep","hello","text.txt", "-A2", NULL};
    execvp(command[0], command);

    return 0;
}