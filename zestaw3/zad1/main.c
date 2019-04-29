#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ftw.h>

int search_dir(char *dirname) {
    DIR *dir = opendir(dirname);
    if(dir == NULL) {
        printf( "Cannot open directory\n");
        return -1;
    }

    struct stat fstat;
    struct dirent *rdir;
    rdir = readdir(dir);
    
    while((rdir = readdir(dir)) != NULL) {
        if(strcmp(rdir->d_name, "..") == 0 || strcmp(rdir->d_name, ".") == 0) {
            continue;
        }
        
        char *new_path = malloc(strlen(dirname) + strlen(rdir->d_name) + 2 );
        sprintf(new_path, "%s/%s", dirname, rdir->d_name);
        if(lstat(new_path, &fstat) == 0) {
            if(S_ISDIR(fstat.st_mode)) {
                pid_t pid = fork();
                int status;
                if(pid == 0) {
                    printf("%s\n", new_path);
                    execlp("/bin/ls", "ls", "-l", new_path, NULL);
                }
                else {
                    printf("pid: %d\n", pid);
                    wait(&status);
                }
                search_dir(new_path);
            }
        }
        free(new_path);
    }
    return 0;
    
}



int main(int argc, char **argv){
    if (argc!=2){
        printf("Bad number of arguments");
        return -1;
    }

    char *path = argv[1];
    char* real_path = realpath(path,NULL);
    search_dir(real_path);
}


