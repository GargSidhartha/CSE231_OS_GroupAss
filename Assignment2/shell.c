#include "headers.h"

void init_shell()
{
    clear();
    printf("\n\n\n\n---------------------------------------------------");
    printf("\n\n\n\t****A SimpleSHELL****");
    printf("\n\n\t-SakshsamSingh\n\tSidharthaGarg-");
    printf("\n\n\n\n---------------------------------------------------");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    
}

int launch(char** args, int arg_num){
    // for (int i = 0; i < arg_num;i++){
    //     printf("%s\n",args[i]);
    // }

    int status = fork();

    if(status < 0){
        printf("Forking failed\n");
    }
    else if(status == 0){
        printf("Child process\n");
        char path[256];
        snprintf(path, sizeof(path), "/usr/bin/%s", args[0]);

        if (execv(path, args) < 0) {
            perror("Command not found");
        }

        exit(1);
    }
    else{
        wait(NULL);
        printf("Parent process\n");
    }

    return 1;
}

int execute(char* command,char** command_history,int history_size){
    int status = 1;
    
    //splitting the command
    char **args;
    int arg_num = 0;

    char *token = strtok(command, " ");
    args = malloc(sizeof(char*));
    while (token != NULL) {
        args[arg_num] = strdup(token);
        arg_num++;
        args = realloc(args, (arg_num + 1) * sizeof(char *));
        token = strtok(NULL, " ");
    }

    args[arg_num - 1][strcspn(args[arg_num - 1], "\n")] = '\0';

    //list down the builtin shell commands here to add support
    if(strcmp(args[0], "exit") == 0) {
        exit(0);
        return 0;
    }
    else if (strcmp(args[0],"history") == 0){
        if (history_size != 0){
            for (int i = 0; i < history_size;i++ ){
                printf("%s",command_history[i]);
            }
        }
        
    }

    else{
        status = launch(args, arg_num);
    }
    

    return status;
}

int main(){
    init_shell();

    int status;
    char** command_history = NULL;
    int history_size = 0;

    do{
        char* user = getenv("USER");
        printf("%s:~$ > ",user);
        char command[COMLEN];

        if(fgets(command,COMLEN,stdin)==NULL){
            if (feof(stdin)) {                      // end of input reached
                status = 1;
                printf("End of file reached. Enter again.\n");
            }
            else if (ferror(stdin)) {               // some other error occurred
                status = 1;
                command[strcspn(command, "\n")] = '\0';
                printf("Some error occurred. Enter again.\n");
            }
        }

        else{
            // command history
            command_history = realloc(command_history, (history_size + 1) * sizeof(char *));
            command_history[history_size] = strdup(command);
            history_size++;
            


            status = execute(command,command_history,history_size);

        }
    }
    while(status);

    
    return 0;
}