#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <time.h>

int state = 0;
int pid;

void handle_stop(int signum) {
    
    //printf("\nOdebrano sygnal %d", signum);
    
    if (!waitpid(pid, NULL, WNOHANG)){
        kill(pid, SIGKILL);
        printf("\nOdebrano sygnał %d \nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n", signum); 
    } 
        
    else{
        printf("resume");
        if(!(pid = fork())){
            execl("./time.sh","./time.sh" , NULL);   
        }
    }
}

void handle_int(int signum){
    printf("Odebrano sygnal %d", signum);
    if(!waitpid(pid, NULL, WNOHANG))
        kill(pid, SIGKILL);
    exit(0);
}


int main(){
    state = 1;
    struct sigaction action;
    action.sa_handler = handle_stop;
    time_t act_time;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    sigaction(SIGTSTP, &action, NULL);
    signal(SIGINT, handle_int);

    if (!(pid = fork())) {
        execl("./time.sh", "./time.sh", NULL);
        printf("%d", pid);
        exit(errno);
    }
    while(1) {}

    
}