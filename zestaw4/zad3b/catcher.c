#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int sig_received = 0;
int mode;

void blockSignals();

void add_handlers();


void kill_all(int pid, int signal, int end_signal) {
    for (int i = 0; i < sig_received; i++)
        kill(pid, signal);
    kill(pid, end_signal);
}


void handle_kill(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGUSR1) {
        sig_received++;
        kill(info->si_pid, SIGUSR1);
    } else {
        kill(info ->si_pid, SIGUSR2);
        printf("Catcher got: %d\n", sig_received);
        exit(0);
    }
}

void handle_sigqueue(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGUSR1) {
        printf("This is: %d\n", info->si_value.sival_int);
        sig_received++;
        union sigval val;
        sigqueue(info->si_pid, SIGUSR2, val);  
    } 
    else {
        union sigval val;
        sigqueue(info->si_pid, SIGUSR2, val);
        printf("Catcher got: %d\n", sig_received);
        exit(1);
    }
}

void handle_sigirt(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGRTMIN) {
        sig_received++;
        kill(info->si_pid, SIGRTMIN);
    } else {
        kill(info->si_pid, SIGRTMAX);
        printf("Catcher got: %d\n", sig_received);
        exit(1);
    }
}



void add_handlers(){
    struct sigaction * sigact = malloc(sizeof(struct sigaction));
    sigact->sa_flags = SA_SIGINFO;
    sigemptyset(&sigact->sa_mask);
    if ((mode) == 0) {
        sigact->sa_sigaction = handle_kill;
        sigaction(SIGUSR1, sigact, NULL);
        sigaction(SIGUSR2, sigact, NULL);
    } else if (mode == 1) {
        sigact->sa_sigaction = handle_sigqueue;
        sigaction(SIGUSR1, sigact, NULL);
        sigaction(SIGUSR2, sigact, NULL);
    } else if (mode == 2) {
        sigact->sa_sigaction = handle_sigirt;
        sigaction(SIGRTMIN, sigact, NULL);
        sigaction(SIGRTMAX, sigact, NULL);
    } else {
        printf("wrong mode");
    }
}




void blockSignals() {
    sigset_t *sig_to_block = malloc(sizeof(sigset_t));
    sigfillset(sig_to_block);
    if (mode== 0) {
        sigdelset(sig_to_block, SIGUSR1);
        sigdelset(sig_to_block, SIGUSR2);
    } else if (mode == 1) {
        sigdelset(sig_to_block, SIGUSR1);
        sigdelset(sig_to_block, SIGUSR2);
    } else if (mode == 2) {
        sigdelset(sig_to_block, SIGRTMIN);
        sigdelset(sig_to_block, SIGRTMAX);
    } else {
        printf("wrong mode");
    }
    sigprocmask(SIG_BLOCK, sig_to_block, NULL);
}


int main(int argc, char **argv) {
    printf("Catcher PID: %d\n", getpid());
    if (argc != 2) {
        printf("Wrong num of arguments");
        return -1;
    }
    char *modearg = argv[1];
    if(strcmp(modearg, "KILL")==0){
        mode =0;
    }
    else if(strcmp(modearg, "SIGQUEUE")==0){
        mode =1;
    }
    else if(strcmp(modearg, "SIGINT")==0){
        mode =2;
    }
    else return -1;

    add_handlers();
    blockSignals();
    while (1);
}
