#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <libgen.h>

int mode;


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


int save_copy(char * copy_name, char * content){
    FILE *fp = fopen(copy_name, "w");
    if(fp == NULL)
        return -1;

    fwrite(content, sizeof(char), strlen(content), fp);
    return fclose(fp);
}


int monitor(char *filename, int time, int path_time, rlim_t cpu_limit, rlim_t memory_limit) {
    int num_of_copies = 0;
    time_t mod_time;
    char *copy_name;
    char *content;

    struct rlimit cpu_rlim;
    struct rlimit memory_rlim;

    cpu_rlim.rlim_cur = cpu_limit;
    cpu_rlim.rlim_max = cpu_limit;

    memory_rlim.rlim_cur = memory_limit;
    memory_rlim.rlim_max = memory_limit;

    if(setrlimit(RLIMIT_CPU, &cpu_rlim) != 0)
        return 2;
    if(setrlimit(RLIMIT_AS, &memory_rlim) != 0)
        return 3;

    if(modification_date(filename, &mod_time) != 0)
        return 0;

    if(mode == 0) {
        int fc = file_content(filename, &content);
        if(fc != 0){
            return 0;
        }
    }
    else {
        pid_t pid = fork();
        if(pid == 0) {
            int copy = get_copy_name(filename, mod_time, &copy_name);
            if (copy != 0){
                return 0;
            }
            execlp("/bin/cp", "cp", filename, copy_name, NULL);
        }
        num_of_copies++;
    }
    time_t last_mod;
    int delta_time = 0;
    while(time--) {
        sleep(1);
        delta_time++;
        if(delta_time == path_time) {
            delta_time = 0;
            if(modification_date(filename, &last_mod) != 0)
                return num_of_copies;
            if(last_mod != mod_time) {
                if(mode == 0) {
                    if(get_copy_name(filename, mod_time, &copy_name) != 0)
                        return num_of_copies;
                    if(save_copy(copy_name, content) != 0)
                        return num_of_copies;
                }
                else {
                    pid_t pid = fork();
                    if(pid == 0) {
                        if(get_copy_name(filename, last_mod, &copy_name) != 0){
                            return num_of_copies;
                        }
                        execlp("/bin/cp", "cp", filename, copy_name, NULL);
                    }
                }
                mod_time = last_mod;
                num_of_copies++;
            }
        }
    }
    return num_of_copies;
}


int main(int argc, char **argv) {

    if(argc < 6) {
        fprintf(stderr, "Bad num of args <file> <time> <mode> <cpu_lim> <mem_limit>");
        return 1;
    }
    char *list = argv[1];
    FILE *file = fopen(list, "r");
    if(file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    int time = atoi(argv[2]);
    mode = atoi(argv[3]);
    rlim_t cpu_limit = atoi(argv[4]);
    rlim_t memory_limit = atoi(argv[5]);

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *paths = malloc(file_size * sizeof(char));
    int number_of_paths = 0;
    fread(paths, sizeof(char), file_size, file);

    char *curr_path = strtok(paths, " \n");

    while(curr_path != NULL) {
        number_of_paths++;
        int path_time = atoi(strtok(NULL, " \n"));
        pid_t pid = fork();
        if(pid == 0){
            return monitor(curr_path, time, path_time, cpu_limit, memory_limit);
        }
        curr_path = strtok(NULL, " \n");
    }
    for (int i =number_of_paths; i > 0; i-- ){
        int status;
        struct rusage before;
        struct rusage after;
        if(getrusage(RUSAGE_CHILDREN, &before) != 0) {
            return 1;
        }
        pid_t pid = wait(&status);
        if(getrusage(RUSAGE_CHILDREN, &after) != 0) {
            return 1;
        }
        if(pid > 0)
            printf("pid %d created %d copies\n", pid, WEXITSTATUS(status));
        else {
            fprintf(stderr, "Error changing process status\n");
            return 1;
        }
        printf("user time: %08ld\n", after.ru_utime.tv_usec - before.ru_utime.tv_usec);
        printf("system time: %08ld\n", after.ru_stime.tv_usec - before.ru_stime.tv_usec);
    }
    return 0;
}