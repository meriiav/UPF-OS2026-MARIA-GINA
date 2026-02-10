#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "splitCommand.h"
#define MAX 1024 //max num of bytes of the input

int main(void){
    char line[MAX];
    char command1[MAX];
    char command2[MAX];

    while(1){
        if (fgets(line, sizeof(line),stdin)==NULL){ //no input
            break;
        }

        if(strcmp(line,"EXIT")==0){
            break; //if execution mode exit, end the process
        }

        else if (strcmp(line,"SINGLE")==0){
            fgets(command1, sizeof(command1),stdin)
            char** argv=split_command(command1);

            pid_t pid=fork();
            if(pid==0){
                execvp(argv[0],argv);
                perror('argv');
                exit(1);
            }
            waitpid(pid,NULL,0)
            free(argv);
        }

        else if (strcmp(line,"CONCURRENT")==0){
            fgets(command1,sizeof(command1),stdin);
            char** argv=split_command(command1);

            pid_t pid= fork();
            if(pid==0){
                execvp(argv[0],argv);
                perror('argv');
                exit(1);
            }
            free(argv); //no waitpid here since there is background execution since concurrency
        }

        else if (strcmp(line,"PIPE")==0){

        }

    }
}