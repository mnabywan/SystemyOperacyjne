#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int state = 0;

void handle_stop(int signum) {
    if(state == 1) {
        printf("\nOdebrano sygnał %d \nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n", signum);
        state = 0;
    }

    else {
        printf("resume");
        state = 1;
    }
}

void handle_int(int signum){
    printf("Odebrano sygnal %d", signum);
    exit(0);
}


int main(){
    state = 1;
    struct sigaction action;
    action.sa_handler = handle_stop;
    time_t act_time;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);


    while(1) {
        sigaction(SIGTSTP, &action, NULL);
        signal(SIGINT, handle_int);

        if (state) {
            char buffer[30];
            act_time = time(NULL);
            strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&act_time));
            printf("%s\n", buffer);
            //printf("%H:%M:%S", localtime(&act_time));
            //printf("%s\n", buffer);
        }
        sleep(1);
        
    }
}