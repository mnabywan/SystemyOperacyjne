
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>



int number_of_signals;
int sig_received = 0;
int catcher_pid;

void handle_kill(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGUSR1) {
        sig_received++;
        if(sig_received < number_of_signals){
            kill(catcher_pid, SIGUSR1);
        } else {
            kill(catcher_pid, SIGUSR2);
        }
    } else {
        printf("Sender got: %d\n", sig_received);
        exit(1);
    }
}

void handle_sigqueue(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGUSR1) {
        sig_received++;
        if(sig_received < number_of_signals){
            union sigval val;
            sigqueue(catcher_pid, SIGUSR1, val);
        } else {
            union sigval val;
            sigqueue(catcher_pid, SIGUSR2, val);
        }
        union sigval val;
        sigqueue(info->si_pid, SIGUSR1, val);
    }
    else {
        printf("Sent: %d  got: %d\n",number_of_signals, sig_received);
        exit(1);
    }
}

void handle_sigirt(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGRTMIN) {
        sig_received++;
        if(sig_received < number_of_signals){
            kill(catcher_pid, SIGRTMIN);
        }
        else{
            kill(catcher_pid, SIGRTMAX);
        }
    } else {
        printf("Sent: %d  got: %d\n",number_of_signals, sig_received);
        exit(1);
    }
}




void add_handlers(int mode){
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
void blockSignals(int mode) {
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




void kill_all(int pid, int signal, int end_signal) {
    for (int i = 0; i < number_of_signals; i++){
        //printf("W KILLALL2");
        printf("NUMBER: %d\n", number_of_signals);
        kill(pid, signal);
        
    }    
    kill(pid, end_signal);
}

void send_signals(int pid,int number_of_signals, int mode){
    if(mode == 0){
        //printf("AAA");
        kill_all(pid, SIGUSR1,SIGUSR2);
    }
    else if(mode == 1){
        union sigval val;
        for(int i = 0; i<number_of_signals; i++){
            sigqueue(pid, SIGUSR1, val);
        }
        sigqueue(pid, SIGUSR2, val);
    }
    else if (mode == 2) {
        kill_all(pid, SIGRTMIN,SIGRTMAX);
    }
}




int main(int argc, char** argv ){
    if (argc != 4){
        printf("Bad number of arguments");
        return -1;
    }

    catcher_pid = atoi(argv[2]);
    number_of_signals = atoi(argv[1]);
    char * modearg = argv[3];
    int mode;    

    if(strcmp(modearg,"KILL")== 0){
        mode =0;
    }

    else if(strcmp(modearg, "SIGQUEUE") == 0){
        mode = 1;
    }
    else if(strcmp(modearg, "SIGIRT")==0){
        mode = 2;
    }
    else{
        printf("Bad mode");
        return -2;
    }

    blockSignals(mode);
    send_signals(catcher_pid,number_of_signals,mode);
    add_handlers(mode);
    while(1);

}
