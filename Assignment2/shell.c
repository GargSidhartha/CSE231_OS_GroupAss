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

int execute(char* command){
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


    if(strcmp(args[0], "exit") == 0) return 0;


    //echo
    else if (strcmp(args[0], "echo") == 0){
        for(int i=1; i<arg_num; i++){
            printf("%s ", args[i]);
        }
        printf("\n");
    }

    //pwd
    else if (strcmp(args[0], "pwd") == 0){
        char cwd[COMLEN];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
    }
    return status;
}

int main(){
    init_shell();

    int status;
    char** command_history = NULL;
    int history_size = 0;

    do{
        printf("kuch bhi: ~$ > ");
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
            // for(int i=0; i<history_size; i++){
            //     printf("%s\n", command_history[i]);
            // }
            // command history

            status = execute(command);

        }
    }
    while(status);

    
    return 0;
}