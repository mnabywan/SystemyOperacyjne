#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

#define SIZE 256

int main(int argc,char **argv){
    if(argc < 2){
        printf("Bad parameters");
        return -1;
    }

    mkfifo(argv[1], S_IWUSR | S_IRUSR);
    char buffer[SIZE];
    FILE * fifo = fopen(argv[1], "r");

    while(fgets(buffer, SIZE, fifo)){
        printf("%s", buffer);
    }

    printf("Master: Ended reading FIFO\n");
    fclose(fifo);
    if (remove(argv[1])){
        printf("Removing FIFO failed! \n");
        exit(1);
    }
    return 0;
}