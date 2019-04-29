#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#define SIZE 256

int main(int argc,char **argv){
    srand(time(NULL));
    if(argc < 3){
        printf("Bad parameters");
        return -1;
    }

    int fifo = open(argv[1], O_WRONLY);
    char buffer1[SIZE];
    char buffer2[SIZE];

    printf("%d\n", getpid());
    int num = (int) strtol(argv[2], NULL, 10);
    for(int i =0; i< num; i++){    
        FILE * date = popen("date", "r");
        fgets(buffer1, SIZE, date);
        int pid = getpid();
        sprintf(buffer2, "Slave: %d - %s", pid, buffer1);
        write(fifo, buffer2, strlen(buffer2));
        fclose(date);
        sleep(rand() % 4 +2);
    }
    
    close(fifo);

    return 0;
}