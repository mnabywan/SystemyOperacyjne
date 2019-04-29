//#include "monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int mode;

int save_copy(char * copy_name, char * content){
    FILE *fp = fopen(copy_name, "w");
    if(fp == NULL)
        return -1;

    fwrite(content, sizeof(char), strlen(content), fp);
    return fclose(fp);
}


int modification_date(char *filename, time_t *mod_time) {
    struct stat file_stat;
    if(lstat(filename, &file_stat) == 0) {
        *mod_time = file_stat.st_mtime;
        return 0;
    }
    return -1;
}


int file_content (char *filename, char ** content){
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    int c_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    *content = malloc(c_size * sizeof(char));
    fread(*content, sizeof(char), c_size, file);
    return fclose(file);
}


int get_copy_name(char *filename, time_t modification_date, char **copy_name) {
    char *parsed_date = malloc(21 * sizeof(char));
    strftime(parsed_date, 21, "_%Y-%m-%d_%H-%M-%S", localtime(&modification_date));
    *copy_name = malloc(strlen(basename(filename)) + 30 );
    sprintf(*copy_name, "archive/%s%s", basename(filename), parsed_date);
    return 0;
}




int monitor(char *path, int time, int path_time){
    int num_of_copies = 0;
    time_t mod_time;
    char * copy_name;
    char *content;

    int md = modification_date(path, &mod_time);
    if(md != 0){
        return 0;
    }

    if (mode ==0){
        int fc = file_content(path, &content);
        if (fc != 0){
            return 0;
        }
    }
    else {
        pid_t pid = fork();
        if(pid == 0){
            //printf("ala");
            int copy = get_copy_name(path, mod_time, &copy_name);
            if (copy != 0){
                return 0;
            }
            execlp("/bin/cp" , "cp", path, copy_name, NULL );
        }
        num_of_copies++;
    }

    time_t last_mod;
    int delta_time =0;
    while(time--){
        sleep(1);
        delta_time++;
        if(delta_time == path_time){
            delta_time = 0;
            if(modification_date(path, &last_mod) != 0){
                return num_of_copies;
            }

            if(last_mod != mod_time){
                if(mode == 0){
                    if(get_copy_name(path,mod_time,&copy_name)!=0){
                        return num_of_copies;
                    }

                    if(save_copy(copy_name, content)!=0){
                        return num_of_copies;
                    }
                }
                else {
                    pid_t pid = fork();
                    if (pid == 0){
                        if(get_copy_name(path, last_mod, &copy_name)!=0){
                            return num_of_copies;
                        }
                        execlp("/bin/cp", "cp", path, copy_name, NULL);
                    }
                }
                mod_time  = last_mod;
                num_of_copies++;
                }

            }
        }
        return num_of_copies;
}


int main(int argc, char** argv){

    if(argc!=4 ){
        printf("Bad number of args");
        return -1;
    }
    char *list = argv[1];
    mode = atoi(argv[3]);
    int time = atoi(argv[2]);


    FILE * file = fopen(list, "r");
    if(file == NULL){
        printf("Cannot open file");
        return 1;
    }

    fseek(file,0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);


    char *paths = calloc(file_size, sizeof(char));
    int number_of_paths = 0;
    fread(paths, sizeof(char), file_size, file);

    char* curr_path = strtok(paths, " \n");


    while(curr_path!= NULL){
        number_of_paths ++;
        int path_time = atoi(strtok(NULL, " \n"));
        pid_t pid = fork();
        if(pid == 0){
            return monitor(curr_path, time, path_time);
        } 
        curr_path = strtok(NULL, " \n");


    }
    for (int i =number_of_paths; i > 0; i-- ){
        int status;
        pid_t pid = wait(&status);
        if(pid > 0)
            printf("Pid %d created %d copies\n", pid, WEXITSTATUS(status));
        else {
            fprintf(stderr, "Error changing process status\n");
            return 1;
        }
   

    } 
    return 0;  
}