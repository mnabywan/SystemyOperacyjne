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
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>


int num=0;
int isWorking = 1;
int isEnded = 0;
int isStop =0;


struct fileData{
    char *path;
    int pid; 
    int stopped;
    int repeatingTime;
    int num_of_copies;
};

struct fileData **data;

struct fileData** files_from_list(char *list){
    FILE *file_list = fopen(list, "r+");
    if(file_list == NULL){
        printf("Cannot open listfile");
        return NULL;
    }
    struct fileData ** data = malloc(sizeof(struct fileData *)* 100);
    for (int i=0; i<100; i++){
        data[i] = malloc(sizeof(struct fileData) + 2 * sizeof(int) + sizeof(char)*100);
    }

    char * current_line = malloc(sizeof(char)*100);
    
    while (fgets(current_line, 100, file_list) != NULL) {
        char *token = strtok(current_line, " ");
        data[num]->path = strdup(token);
        //printf("%s\n", data[num]->path);
        char *ptr = strtok(NULL, " ");
        //printf("%s\n", ptr);
        //data[num]->repeatingTime = atoi(ptr);
        //printf("%d\n", data[num]-> repeatingTime);
        num++;

        
    }

    fclose(file_list);
    return data;
}



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


char* file_content (char *filename){
    struct stat fileinfo;
    if (lstat(filename, &fileinfo) != 0) {
        perror("Couldnt read file");
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Couldnt read file");
    }

    char *content = malloc(fileinfo.st_size + 1);
    if (fread(content, 1, fileinfo.st_size, file) != fileinfo.st_size) {
        perror("Could not read file");
    }

    content[fileinfo.st_size] = '\0';
    fclose(file);
    return content; 
}




char *get_file_name(char *filePath) {
    char *fileName = strchr(filePath, '/');
    if (fileName == NULL) {
        perror("Something went wrong with parsing name of file");
    }
    return fileName + 1;
}

void start(int signum){
    isWorking =1;
}


void stop(int signum){
    isWorking =0;
}

void end(int sig) {
    isEnded = 1;
}

void stop_handler(int signum) {
    if (signum == SIGINT){
        isStop = 1;
    }
}


void watchFile(struct fileData *data, char *list){
    signal(SIGUSR1, &stop);
    signal(SIGUSR2, &start);
    signal(SIGRTMIN, &end);


    //int numOfCopies = 0;
    struct stat file_stats;
    
    if (lstat(data->path, &file_stats) == -1) {
        printf("File: %s", data->path);
        perror("Cannot read file");
    }

    time_t last_mod = file_stats.st_mtime;

    char *content = file_content(data->path);
    
    while (1) {
        if(isEnded == 1){
            break;
        }
        
        while(isWorking == 1){
            if (lstat(data->path, &file_stats) == -1) {
                printf("File: %s", data->path);
                perror("Coudnt read file");
            }
            char *modificationTime = malloc(sizeof(char) * 1000);
            strftime(modificationTime, 1000, "_%Y-%m-%d_%H-%M-%S", localtime(&file_stats.st_mtime));
            char *newFileName = malloc(1000 * sizeof(char));
            sprintf(newFileName, "archiwum/%s%s", get_file_name(data->path), modificationTime);
            if (last_mod < file_stats.st_mtime) {
                DIR *dir = opendir("archiwum");
                if (!dir) {
                    mkdir("archiwum", 0777);
                }
                FILE *file = fopen(newFileName, "w");
                if (!file) {
                    perror("Coudlnt created file");
                }
                fwrite(content, 1, strlen(content), file);
                fclose(file);
                last_mod = file_stats.st_mtime;
                content = file_content(data->path);
                
                closedir(dir);
            }
            free(modificationTime);
            free(newFileName);
            sleep(data->repeatingTime);
            if(isEnded == 1){
                break;
            }
        }
        if(isWorking == 0){
            sleep(1);
        }   
    }
    free(content);
    exit(data->num_of_copies);
}


void monitor (struct fileData ** data, char * list){
    while(1){
        char * val = malloc(sizeof(char)*64);
        fgets(val,20, stdin);
        if(strcmp(val, "END\n") == 0 || isStop == 1){
            for (int i =0; i< num; i++){
                int status;
                int n = kill(data[i]->pid, SIGRTMIN);
                waitpid(data[i]->pid, &status, 0);
                printf("Process: %d file: %s , copies: %d\n", data[i]->pid,data[i]->path, WEXITSTATUS(status));
            }
            free(val);
            exit(1);
        }

        else if(strcmp(val, "LIST\n") == 0){
            for (int i = 0; i < num; i++) {
                printf("Process: %d file: %s , is stopped: %d\n", data[i]->pid, data[i]->path, data[i]->stopped);

            }

        }else if (strcmp(val, "STOP ALL\n") == 0) {
            for (int i = 0; i < num; i++) {
                kill(data[i]->pid, SIGUSR1);
                data[i]->stopped = 1;
            }
        } else if (strcmp(val, "START ALL\n") == 0) {
            for (int i = 0; i < num; i++) {
                kill(data[i]->pid, SIGUSR2);  
                data[i]->stopped = 0;            
            }
        }

        else if(strncmp(val, "START", 5 )==0){
            strtok(val, " ");
            char *ptr = strtok(NULL, " ");
            if (ptr != NULL) {
                int pid = atoi(ptr);
                int found = 0;
                for (int i = 0; i < num; i++) {
                    if (data[i]->pid == pid) {
                        kill(pid, SIGUSR2);
                        data[i]->stopped = 0;
                        found =1;
                    }
                }

                
            }
            else printf("Bad pid");
            free(val);
        }

        else if (strncmp(val,"STOP", 4)==0){
            strtok(val, " ");
            char *ptr = strtok(NULL, " ");
            if (ptr != NULL) {
                int pid = atoi(ptr);
                int found = 0;
                for (int i = 0; i < num; i++) {
                    if (data[i]->pid == pid) {
                        data[i]->stopped = 1;
                        found = 1;
                        kill(pid, SIGUSR1);
                        
                    }
                }
            }
        
            else{
             printf("Blad");
            }

            free(val);
        }   


}

}


void createProcesses(struct fileData **fileData, char *list) {
    for (int i = 0; i < num; i++) {
        pid_t curr = fork();
        if (curr == 0) {
            watchFile(fileData[i],list);
        } else {
            fileData[i]->pid = curr;
        }
    }
    monitor(fileData, list);
}

int main(int argc, char** argv){

    if(argc!=2 ){
        printf("Bad number of args");
        return -1;
    }

    struct sigaction act;
    act.sa_handler = stop_handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    char *list = argv[1];
    data = files_from_list(list); 
    createProcesses(data, list);
    for (int i = 0; i < 100; i++) {
        free(data[i]);
    }
    free(data);


 
}