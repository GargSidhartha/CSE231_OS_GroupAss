#include "headers.h"

void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****MY SHELL****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    
}

int main(){
    init_shell();

    int status;
    do{
        printf("kuch bhi: ~$");
        char* command;
        if(fgets(command,COMLEN,stdin);

    }
    while(status);

    
    return 0;
}