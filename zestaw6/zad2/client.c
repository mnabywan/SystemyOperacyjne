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
#include <mqueue.h>
#include "chat.h"

#include <mqueue.h>
mqd_t server_queueID, client_queueID;
int clientID;
pid_t child;
char *text;

void send_to_client(int id, long type, long value, int textSize) {
    Message message;
    message.type = type;
    message.value = value;
    message.textSize = textSize;
    memcpy(message.text, "", 1);

    if (textSize > MAX_MESSAGE_LEN) {
        printf("Message too long\n");
        exit(-1);
    }
    if (textSize > 0) {
        memcpy(message.text, text, textSize);
    }

    printf("\nMessage -- Type: %ld Value: %ld Text: %s\n", message.type, message.value, message.text);

    if (mq_send(id, (char *)&message, MAX_MESSAGE_SIZE, message.type) == -1){
        printf("cannot send to client");
        exit(1);
    }
}

Message receiveData(int id) {
    Message message;
    if (mq_receive(id, (char *)&message, MAX_MESSAGE_SIZE, NULL) == -1){
        printf("Canntot receivreceive");
        exit(1);
    }
    memcpy(text, message.text, message.textSize);
    return message;
}


void quit_queue() {
    if (child != 0) {
        mq_close(client_queueID);
        char path[15];
        sprintf(path, "/%d", getpid());
        mq_unlink(path);
        send_to_client(server_queueID, STOP, clientID, 0);
        mq_close(server_queueID);
        exit(1);
    }
}

void handleCtrlC(int signum) {
    printf("CTRLC PRESSED\n");
    exit(0);
}

void handleSIGUSR(int signum) {
    printf("Server stopped\n");
    exit(0);
}

void do_echo(char *string, size_t size) {
    memcpy(text, string + 5, size);
    send_to_client(server_queueID, ECHO, clientID, size);
}

void do_list() {
    send_to_client(server_queueID, LIST, 0, 0);
}

void do_2all(char *string, size_t size) {
    memcpy(text, string + 5, size);
    send_to_client(server_queueID, TO_ALL, clientID, size);
}

void do_2one(char *string, int id, size_t size) {
    memcpy(text, string, size);
    send_to_client(server_queueID, TO_ONE, id, size);
}

void do_friends(char *string, size_t size) {
    memcpy(text, string + 8, size);
    send_to_client(server_queueID, FRIENDS, clientID, size);
}

void do_add_friends(char *string, size_t size) {
    memcpy(text, string + 4, size);
    send_to_client(server_queueID, ADD, clientID, size);
}

void do_del_friends(char *string, size_t size) {
    memcpy(text, string + 4, size);
    send_to_client(server_queueID, DEL, clientID, size);
}

void do_2friends(char *string, size_t size) {
    memcpy(text, string + 9, size);
    send_to_client(server_queueID, TO_FRIENDS, clientID, size);
}

void do_input(char *comm, size_t size) {
    if (strncmp(comm, "ECHO", 4) == 0) {
        do_echo(comm, size - 5);
    } else if (strncmp(comm, "LIST", 4) == 0) {
        do_list();
    } else if (strncmp(comm, "2ALL", 4) == 0) {
        do_2all(comm, size - 5);
    } else if (strncmp(comm, "2ONE", 4) == 0) {
        strtok(comm, " ");
        char *str = strtok(comm + 5, " ");
        if (str == NULL) {
            fprintf(stderr, "Wrong number of arguments\n");
            return;
        }
        int id = atoi(str);
        str = strtok(NULL, " ");
        size_t size = 0;
        if (str != NULL)
            size = strlen(str);
        do_2one(str, id, size);
    } else if (strncmp(comm, "FRIENDS", 7) == 0) {
        do_friends(comm, size - 8);
    } else if (strncmp(comm, "ADD", 3) == 0) {
        do_add_friends(comm, size - 4);
    } else if (strncmp(comm, "DEL", 3) == 0) {
        do_del_friends(comm, size - 4);
    } else if (strncmp(comm, "2FRIENDS", 8) == 0) {
        do_2friends(comm, size - 9);
    } else if (strncmp(comm, "STOP", 4) == 0) {
        exit(0);
    } else {
        printf("Wrong command %s\n", comm);
    }
}


int main(int argc, char **argv) {
    text = malloc(MAX_MESSAGE_LEN);
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    char path[20];
    sprintf(path, "/%d", getpid());
    if((client_queueID = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1){
        printf("Error in mqopen client");
        exit(1);
    }
    if((server_queueID = mq_open(SERVER, O_WRONLY)) == -1){
       printf("Error in mqopen server");
        exit(1);
    }
    atexit(quit_queue);

    send_to_client(server_queueID, INIT, getpid(), 0);
    Message message = receiveData(client_queueID);
    clientID = message.value;

    printf("ID: %d\n", clientID);
    if ((child = fork()) == 0) {
        while (1) {
            Message received = receiveData(client_queueID);
            if (received.type == ECHO){
                printf("Message Echo: %s\n", received.text);
                printf("--\n");
            }
            if (received.type == STOP) {
                printf("Message STOP");
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
            if(strcmp(argv[1], "READ") != 0)
                printf("Error in read file");

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

