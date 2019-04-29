#define _XOPEN_SOURCE 500

#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>

const char format[] = "%Y-%m-%d %H:%M:%S";
time_t gdate;
char *gsign;

void print_info(const char *name, const struct stat* stats){
    char buff[255];
    if(S_ISSOCK(stats->st_mode)){
        printf("Socket ");
    }
    else if(S_ISDIR(stats->st_mode)){
        printf("Directory ");
    }

    else if(S_ISREG(stats->st_mode)){
        printf("File  ");
    }

    else if(S_ISLNK(stats->st_mode)){
        printf("Link  ");
    }

    else if(S_ISFIFO(stats->st_mode)){
        printf("Fifo  ");
    }
    else if(S_ISBLK(stats->st_mode)){
        printf("Block dev  ");
    }
    else if(S_ISCHR(stats->st_mode)){
        printf("Char dev  ");
    }
    
    
  

    printf("Size: %ld", stats->st_size);
    printf("Access time: %s   Mod time: %s     ", ctime(&stats->st_atime), ctime(&stats->st_mtime));
    printf("%s ", name);

    printf("\n");
    printf("-----------------------\n");
    //printf("%s",buff);


}





static int nftw_search(const char *path, const struct stat *stats, int typeflag, struct FTW *ftwbuf){
    struct tm mtime;
    
    (void) localtime_r(&stats->st_mtime, &mtime);

    if (typeflag != FTW_F) {
        return 0;
    }

    int diff = difftime(gdate, stats->st_mtime);
    if (!(
            (diff == 0 && strcmp(gsign, "=") == 0)
            || (diff > 0 && strcmp(gsign, "<") == 0)
            || (diff < 0 && strcmp(gsign, ">") == 0)
    )) {
        return 0;
    }


    print_info(path, stats);
    return 0;






}




void search_dir(char *path, char* sign, time_t date){
    
    if(path == NULL){
        printf("Nullpath");
    }
    DIR *dir = opendir(path);

    if (dir == NULL){
        printf("Cannot open directory, error\n");
        return ;
    }

    struct dirent *rdir = readdir(dir);
    struct stat buff;

    char new_path[256];
    while (rdir != NULL){
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, rdir->d_name);
    
        lstat(new_path, &buff);
        

        
        if(strcmp(rdir->d_name, ".")==0 || strcmp(rdir->d_name, "..") ==0 ){
            rdir = readdir(dir);
            continue;
             
            
        }
        else  {
            if (S_ISREG(buff.st_mode)){
                int diff = difftime(date, buff.st_mtime);
                

                if(strcmp(sign, "=")==0 && diff == 0){
                    print_info(new_path, &buff);
                }
                else if(strcmp(sign, "<")==0 && diff > 0){
                    print_info(new_path, &buff);
                }
                else if(strcmp(sign, ">")==0 && diff < 0){
                    print_info(new_path, &buff);
                }
                
            }
            if(S_ISDIR(buff.st_mode)){
                search_dir(new_path, sign, date);
            }
            rdir = readdir(dir);

        }
    }
    closedir(dir);
    
}
int main(int argc, char **argv){
    if (argc < 4){
        printf("Too few arguments");
        return -1;
    }

        srand(time(NULL));
    
    char *path = argv[1];
    char *sign = argv[2];
    char *udate = argv[3];

     DIR *dir = opendir(realpath(path, NULL));
    if (dir == NULL) {
        printf("Cannot open the directory\n");
        return 1;
    }

    struct tm *timestamp = malloc(sizeof(struct tm));

    strptime(udate, format, timestamp);
    time_t date = mktime(timestamp);
    
    search_dir(realpath(path,NULL),sign, date);
    gdate = date;
    gsign= sign;
    nftw(realpath(path, NULL), nftw_search, 100, FTW_PHYS);



}