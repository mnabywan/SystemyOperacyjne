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

int client_queueID;
int server_queueID;
int clientID;
pid_t childPID;
char *text;

void send_to_client(int id, long type, long value, int textSize) {
    Message message;
    message.type = type;
    message.value = value;
    message.textSize = textSize;
    memcpy(message.text, "", 1);

    if (textSize > MAX_MESSAGE_LEN) {
        printf("Message too long\n");
        exit(1);
    }
    if (textSize > 0) {
        memcpy(message.text, text, textSize);
    }

    printf("\nMessage --- Type: %ld Value: %ld Text: %s\n", message.type, message.value, message.text);

    if (msgsnd(id, &message, MAX_MESSAGE_SIZE, 0) == -1){
        printf("Coudlnt sent to client");
        exit(1);
    }
}

Message receive_data(int id) {
    Message message;
    if (msgrcv(id, &message, MAX_MESSAGE_SIZE, -100,0) == -1){
        printf("cannot receive");
        exit(1);
}
    memcpy(text, message.text, message.textSize);
    return message;
}

void quit() {
    if (childPID != 0) {
        msgctl(client_queueID, IPC_RMID, NULL);
        send_to_client(server_queueID, STOP, clientID, 0);
        exit(1);
    }
}

void handleCtrlC(int signum) {
    printf("CTRLC PRESSED\n");
    exit(0);
}

void handleSIGUSR(int signum) {
    printf("SERVER STOPPED\n");
    exit(0);
}


void send_echo(char *string, size_t size) {
    memcpy(text, string + 5, size);
    send_to_client(server_queueID, ECHO, clientID, size);
}

void send_list() {
    send_to_client(server_queueID, LIST, 0, 0);
}

void send_2one(char *string, int id, size_t size) {
    memcpy(text, string, size);
    send_to_client(server_queueID, TO_ONE, id, size);
}

void send_friends(char *string, size_t size) {
    memcpy(text, string + 8, size);
    send_to_client(server_queueID, FRIENDS, clientID, size);
}

void send_add_friends(char *string, size_t size) {
    memcpy(text, string + 4, size);
    send_to_client(server_queueID, ADD, clientID, size);
}

void send_del_friends(char *string, size_t size) {
    memcpy(text, string + 4, size);
    send_to_client(server_queueID, DEL, clientID, size);
}

void send_2all(char *string, size_t size) {
    memcpy(text, string + 5, size);
    send_to_client(server_queueID, TO_ALL, clientID, size);
}


void send_2friends(char *string, size_t size) {
    memcpy(text, string + 9, size);
    send_to_client(server_queueID, TO_FRIENDS, clientID, size);
}

void do_input(char *comm, size_t size) {
    if (strncmp(comm, "ECHO", 4) == 0) {
        send_echo(comm, size - 5);
    } else if (strncmp(comm, "LIST", 4) == 0) {
        send_list();
    } else if (strncmp(comm, "2ALL", 4) == 0) {
        send_2all(comm, size - 5);
    } else if (strncmp(comm, "2ONE", 4) == 0) {
        strtok(comm, " ");
        char *str = strtok(comm + 5, " ");
        if (str == NULL) {
            printf("Wrong number of arguments\n");
            exit(1);
        }
        int id = atoi(str);
        str = strtok(NULL, " ");
        size_t size = 0;
        if (str != NULL)
            size = strlen(str);
        send_2one(str, id, size);
    } else if (strncmp(comm, "FRIENDS", 7) == 0) {
        send_friends(comm, size - 8);
    } else if (strncmp(comm, "ADD", 3) == 0) {
        send_add_friends(comm, size - 4);
    } else if (strncmp(comm, "DEL", 3) == 0) {
        send_del_friends(comm, size - 4);
    } else if (strncmp(comm, "2FRIENDS", 8) == 0) {
        send_2friends(comm, size - 9);
    } else if (strncmp(comm, "STOP", 4) == 0) {
        exit(0);
    } else {
        printf("Wrong command %s\n", comm);
    }
}





int main(int argc, char **argv) {
    text = malloc(MAX_MESSAGE_LEN);
    if ((server_queueID = msgget(ftok(getenv("HOME"), 0), 0)) == -1){
        printf("Cannot get server queue");
        exit(1);
    }
    if ((client_queueID = msgget(ftok(getenv("HOME"), getpid()), IPC_CREAT | 0666)) == -1){
        printf("Cannot get client queue");
        exit(1);
    }
    atexit(quit);

    send_to_client(server_queueID, INIT, ftok(getenv("HOME"), getpid()), 0);
    Message message = receive_data(client_queueID);
    clientID = message.value;

    printf("ID: %d\n", clientID);
    if ((childPID = fork()) == 0) {
        while (1) {
            Message received = receive_data(client_queueID);
            if (received.type == ECHO){
                printf("Message ECHO: %s\n", received.text);
                printf("\n");
            }
            if (received.type == STOP) {
                printf("Message STOP\n");
                kill(getppid(), SIGUSR1);
                exit(0);
            }
        }
    } else {
        signal(SIGINT, handleCtrlC);
        signal(SIGUSR1, handleSIGUSR);
        char *comm = malloc(MAX_MESSAGE_LEN);
        if (argc > 2) {
            FILE *file;
            if(strcmp(argv[1], "READ") != 0){
                printf("Bad read");
                exit(1);
            }

            if ((file = fopen(argv[2], "r")) != NULL) {
                char *input = malloc(MAX_FILE_SIZE);
                fread(input, sizeof(char), MAX_FILE_SIZE, file);
                comm = strtok(input, "\n");
                while (comm != NULL) {
                    do_input(comm, strlen(comm));
                    comm = strtok(NULL, "\n");
                }
            }
        }
        size_t MAX_MESSAGE_LEN_SIZE_T = MAX_MESSAGE_LEN + 6;
        while (1) {
            size_t size = getline(&comm, &MAX_MESSAGE_LEN_SIZE_T, stdin);
            do_input(comm, size);
        }
    }
}


