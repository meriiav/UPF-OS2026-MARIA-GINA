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
            free(argv); //no waitpid here since there is background execution due to concurrency
        }

        else if (strcmp(line,"PIPE")==0){
            fgets(command1, sizeof(command1), stdin);
            fgets(command2, sizeof(command2), stdin);

            char **argv1 = split_command(command1);
            char **argv2 = split_command(command2);

            int fd[2];
            pipe(fd);

            pid_t pid1 = fork();
            if (pid1 == 0) {
                //child1: writes to pipe
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);

                execvp(argv1[0], argv1);
                perror("execvp");
                exit(1);
            }

            pid_t pid2 = fork();
            if (pid2 == 0) {
                //child2: reads from pipe
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                close(fd[0]);

                execvp(argv2[0], argv2);
                perror("execvp");
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);

            //waiting for both to end
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

            free(argv1);
            free(argv2);
        }
    }
}