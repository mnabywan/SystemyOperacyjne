#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define MAX_LEN  100
#define MAX_NUM_ARGS  32
#define MAX_PROGS 6


char *progs[MAX_PROGS][MAX_PROGS];

int parse_programs(char *buffer){
    char* ptr = strtok(buffer, " ");
    int num =0;
    int j =0;
     
    while (ptr!= NULL){
        if (ptr[0] == '|' && ptr[1] == '\0'){
            progs[num][j] = NULL;
            num++;
            j = 0;
        }
        else{
            progs[num][j] = ptr;
            j++;
        }
        ptr = strtok(NULL, " \n");
    }
    return num;
}




int main(int argc, char **argv){
    char buffer[MAX_LEN];
    int fd1[2];
    int fd2[2];
    int pid[256];

    int i=0;
    int j;
    int num_of_args = 0;
    int status;

    if  (argc < 2 ){
        printf("Bad number of arguments\n");
        return -1;
    }


    FILE * file = fopen(argv[1], "r");
    if(!file){
        printf("cannot open file\n");
        return -1;
    }

    while(1){
        num_of_args = parse_programs(buffer);
        printf("COMMAND: %s\n", buffer);
        for (int i =0; i<=num_of_args; i++){
            if(pipe(fd1) == -1){
                printf("Error in pipe\n");
                return -1;
            }
            pid[i] = fork();
            if(pid[i] == 0){
                if (i>0){ 
                    dup2(fd2[0], STDIN_FILENO);
                    close(fd2[1]);
                    close(fd2[0]);

                }

                if(i != num_of_args){
                    dup2(fd1[1],STDOUT_FILENO);
                    close(fd1[0]);
                    close(fd1[1]);
                }

                execvp(progs[i][0],progs[i]);
                exit(0);
            }

            else if(pid[i] > 0){
                if(i > 0){
                    close(fd2[0]);
                    close(fd2[1]);
                }
                if(i < num_of_args){
                    fd2[0] = fd1[0];
                    fd2[1] = fd1[1];
                }
            }
        }
        for(i = 0; i < num_of_args; i++) waitpid(pid[i],&status,0);
        if(fgets(buffer,512 * sizeof(char),file) == NULL) break;
    }


}