#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "chat.h"

int server_queueID;

int clients_queueID[MAX_CLIENTS];
int friends_number = 0;
int friends[MAX_CLIENTS];
int last_client = 0;
char* text;


size_t add_time_to_message(Message message){
    time_t now;
    time(&now);
    return sprintf(text, "text: %s time: %s ", message.text, ctime(&now));
}

void send_to_client(int id, long type, long value, int textSize) {
    Message message;
    message.type = type;
    message.value = value;
    message.textSize = textSize;
    if (textSize > MAX_MESSAGE_LEN) {
        printf("Msg too long");
        exit(1);
    }
    if (textSize > 0) {
        memcpy(message.text, text, textSize);
    }
    if (msgsnd(id, &message, MAX_MESSAGE_SIZE, 0) == -1){
        printf("Cannot send");
        exit(1);
    }
}

Message receive_data(int server_qID) {
    Message message;
    if (msgrcv(server_qID, &message, MAX_MESSAGE_SIZE, -100, 0) == -1){
        printf("Cannot receive");
        exit(1);
    }
        
    memcpy(text, message.text, message.textSize);
    return message;
}


void do_init(Message message) {
    if(last_client >= MAX_CLIENTS){
        printf("Cannot init, too many clients");
        exit(1);
    }
    if ((clients_queueID[last_client] = msgget(message.value, 0)) == -1){
        printf("Cannot get ");
        exit(1);
    }
    send_to_client(clients_queueID[last_client], INIT, last_client, 0);
    last_client++;
}

void do_echo(Message message) {
    send_to_client(clients_queueID[message.value], ECHO, 0, add_time_to_message(message));
}

void do_2one(Message message) {
    send_to_client(clients_queueID[message.value], ECHO, 0, add_time_to_message(message));
}

void do_list() {
    printf("Clinets:\n");
    for (int i = 0; i < last_client; i++) {
        if (clients_queueID[i] != 0) {
            printf("Id: %d, Queued: %d\n", i, clients_queueID[i]);
        }
    }
}

void do_2all(Message message) {
    size_t len = add_time_to_message(message);
    for (int i = 0; i < last_client; i++) {
        if (clients_queueID[i] != 0) {
            send_to_client(clients_queueID[i], ECHO, 0, len);
        }
    }
}

void do_friends(Message message) {
    char *tmp = malloc(message.textSize);
    memcpy(tmp, message.text, message.textSize);
    char *str = strtok(tmp, " ");
    friends_number = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        friends[i] = -1;
    }
    while (str != NULL) {
        int friend_added = 0;
        int num = atoi(str);
        for (int i = 0; i < friends_number; i++) {
            if (friends[i] == num) {
                friend_added = 1;
            }
        }
        if (friend_added) {
            printf("Friend already added");
            exit(1);
        }
        friends[friends_number] = num;
        friends_number++;
        if(friends_number >= MAX_CLIENTS){
            printf("to many");
            exit(1);
        }
        str = strtok(NULL, " ");
    }
}

void do_add_friends(Message message) {
    char *tmp = malloc(message.textSize);
    memcpy(tmp, message.text, message.textSize);
    char *str = strtok(tmp, " ");
    while (str != NULL) {
        int currentNumExists = 0;
        int num = atoi(str);
        for (int i = 0; i < friends_number; i++) {
            if (friends[i] == num) {
                currentNumExists = 1;
            }
        }
        if (currentNumExists) {
            printf("You doubled friends");
            exit(1);
        }
        friends[friends_number] = num;
        friends_number++;

        if(friends_number >= MAX_CLIENTS){
            printf("Too many friends");
            exit(1);
        }
        str = strtok(NULL, " ");
    }
}

void do_del_friends(Message message) {
    char *tmp = malloc(message.textSize);
    memcpy(tmp, message.text, message.textSize);
    char *str = strtok(tmp, " ");
    while (str != NULL) {
        int num = atoi(str);
        for (int i = 0; i < friends_number; i++) {
            if (friends[i] == num) {
                for (int j = i; j < friends_number - 1; j++) {
                    friends[j] = friends[j + 1];
                }
                friends_number--;
            }
        }
        str = strtok(NULL, " ");
    }
}

void do_2friends(Message message) {
    size_t len = add_time_to_message(message);
    for (int i = 0; i < friends_number; i++) {
        if (clients_queueID[friends[i]] != 0) {
            send_to_client(clients_queueID[friends[i]], ECHO, 0, len);
        }
    }
}

void do_stop(Message message) {
    clients_queueID[message.value] = 0;
}

void do_receive(Message message) {
    switch (message.type) {
        case INIT:
            do_init(message);
            break;
        case ECHO:
            do_echo(message);
            break;
        case TO_ONE:
            do_2one(message);
            break;
        case LIST:
            do_list();
            break;
        case TO_ALL:
            do_2all(message);
            break;
        case FRIENDS:
            do_friends(message);
            break;
        case ADD:
            do_add_friends(message);
            break;
        case DEL:
            do_del_friends(message);
            break;
        case TO_FRIENDS:
            do_2friends(message);
            break;
        case STOP:
            do_stop(message);
            break;
    }
}

void handleCtrlC(int signum) {
    printf("CTRL C PRESSED");
    exit(0);
}

void quit_queue() {
    for (int i = 0; i < last_client; i++) {
        if (clients_queueID[i] != 0) {
            send_to_client(clients_queueID[i], STOP, 0, 0);
            receive_data(server_queueID);
        }
    }
    msgctl(server_queueID, IPC_RMID, NULL);
}



int main() {
    text = malloc(MAX_MESSAGE_LEN);
    for(int i = 0; i < MAX_CLIENTS; i++){
        friends[i] = -1;
    }
    signal(SIGINT, handleCtrlC);
    //tworzymt kolejke na zecenie dla servera
    server_queueID = msgget(ftok(getenv("HOME"), 0), IPC_CREAT | 0666);
    if (server_queueID == -1){
        printf("Cannot create server queue");
        exit(1);
    }
    atexit(quit_queue);

    Message message;
    printf("Start server: %d\n", server_queueID);

    while (1) {
        message = receive_data(server_queueID);
        printf("Mesage: Type: %ld Value: %ld Text: %s\n", message.type, message.value, message.text);
        do_receive(message);
    }
}


