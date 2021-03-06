#include <pthread.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

int sock;
char *name;

void init(char *, char *, char *);

SocketMessage getMessage(void);

void deleteMessage(SocketMessage);

void send_msg(SocketMessage);

void sendEmptyMessage(SocketMessageType);

void sendDoneMessage(int, char *);

void handleINT(int);

void cleanup(void);

int main(int argc, char *argv[]) {
    if (argc != 4) die("Pass: client name, variant (WEB/UNIX), address");

    init(argv[1], argv[2], argv[3]);

    while (1) {
        SocketMessage msg = getMessage();

        switch (msg.type) {
            case OK: {
                break;
            }
            case PING: {
                sendEmptyMessage(PONG);
                break;
            }
            case NAME_TAKEN:
                die("Name is already taken");
            case FULL:
                die("Server is full");
            case WORK: {
                puts("Doing work...");
                char *buffer = malloc(100 + 2 * msg.size);
                if (buffer == NULL) die_errno();
                sprintf(buffer, "echo '%s' | awk '{for(x=1;$x;++x)print $x}' | sort | uniq -c", (char *) msg.content);
                FILE *result = popen(buffer, "r");
                if (result == 0) {
                    free(buffer);
                    break;
                }
                int n = fread(buffer, 1, 99 + 2 * msg.size, result);
                buffer[n] = '\0';
                puts("Work done...");
                sendDoneMessage(msg.id, buffer);
                free(buffer);
                break;
            }
            default:
                break;
        }

        deleteMessage(msg);
    }
}

void init(char *n, char *variant, char *address) {
    // register atexit
    if (atexit(cleanup) == -1) die_errno();

    // register int handler
    if (signal(SIGINT, handleINT) == SIG_ERR)
        show_errno();

    // set name
    name = n;

    // parse address
    if (strcmp("WEB", variant) == 0) {
        strtok(address, ":");
        char *port = strtok(NULL, ":");
        if (port == NULL) die("Specify a port");

        uint32_t in_addr = inet_addr(address);
        if (in_addr == INADDR_NONE) die("Invalid address");

        uint16_t port_num = (uint16_t) parse_pos_int(port);
        if (port_num < 1024)
            die("Invalid port number");

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            die_errno();

        struct sockaddr_in web_addr;
        memset(&web_addr, 0, sizeof(struct sockaddr_in));

        web_addr.sin_family = AF_INET;
        web_addr.sin_addr.s_addr = in_addr;
        web_addr.sin_port = htons(port_num);

        if (connect(sock, (const struct sockaddr *) &web_addr, sizeof(web_addr)) == -1)
            die_errno();
    } else if (strcmp("UNIX", variant) == 0) {
        char *un_path = address;

        if (strlen(un_path) < 1 || strlen(un_path) > UNIX_PATH_MAX)
            die("Invalid unix socket path");

        struct sockaddr_un un_addr;
        un_addr.sun_family = AF_UNIX;
        snprintf(un_addr.sun_path, UNIX_PATH_MAX, "%s", un_path);

        struct sockaddr_un client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sun_family = AF_UNIX;
        snprintf(client_addr.sun_path, UNIX_PATH_MAX, "%s", name);

        if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
            die_errno();

        if (bind(sock, (const struct sockaddr *) &client_addr, sizeof(client_addr)) == -1)
            die_errno();

        if (connect(sock, (const struct sockaddr *) &un_addr, sizeof(un_addr)) == -1)
            die_errno();
    } else {
        die("Unknown variant");
    }

    sendEmptyMessage(REGISTER);
}

void send_msg(SocketMessage msg) {
    ssize_t head_size = sizeof(msg.type) + sizeof(msg.size) + sizeof(msg.nameSize) + sizeof(msg.id);
    ssize_t size = head_size + msg.size + 1 + msg.nameSize + 1;
    int8_t *buff = malloc(size);
    if (buff == NULL) die_errno();

    memcpy(buff, &msg.type, sizeof(msg.type));
    memcpy(buff + sizeof(msg.type), &msg.size, sizeof(msg.size));
    memcpy(buff + sizeof(msg.type) + sizeof(msg.size), &msg.nameSize, sizeof(msg.nameSize));
    memcpy(buff + sizeof(msg.type) + sizeof(msg.size) + sizeof(msg.nameSize), &msg.id, sizeof(msg.id));

    if (msg.size > 0 && msg.content != NULL)
        memcpy(buff + head_size, msg.content, msg.size + 1);
    if (msg.nameSize > 0 && msg.name != NULL)
        memcpy(buff + head_size + msg.size + 1, msg.name, msg.nameSize + 1);

    if (write(sock, buff, size) != size) die_errno();

    free(buff);
}

void sendEmptyMessage(SocketMessageType type) {
    SocketMessage msg = {type, 0, strlen(name), 0, NULL, name};
    send_msg(msg);
};

void sendDoneMessage(int id, char *content) {
    SocketMessage msg = {WORK_DONE, strlen(content), strlen(name), id, content, name};
    send_msg(msg);
}

SocketMessage getMessage(void) {
    SocketMessage msg;
    ssize_t head_size = sizeof(msg.type) + sizeof(msg.size) + sizeof(msg.nameSize) + sizeof(msg.id);
    int8_t buff[head_size];
    if (recv(sock, buff, head_size, MSG_PEEK) < head_size)
        die("Uknown message from server");

    memcpy(&msg.type, buff, sizeof(msg.type));
    memcpy(&msg.size, buff + sizeof(msg.type), sizeof(msg.size));
    memcpy(&msg.nameSize, buff + sizeof(msg.type) + sizeof(msg.size), sizeof(msg.nameSize));
    memcpy(&msg.id, buff + sizeof(msg.type) + sizeof(msg.size) + sizeof(msg.nameSize), sizeof(msg.id));

    ssize_t size = head_size + msg.size + 1 + msg.nameSize + 1;
    int8_t *buffer = malloc(size);

    if (recv(sock, buffer, size, 0) < size) {
        die("Uknown message from server");
    }

    if (msg.size > 0) {
        msg.content = malloc(msg.size + 1);
        if (msg.content == NULL) die_errno();
        memcpy(msg.content, buffer + head_size, msg.size + 1);
    } else {
        msg.content = NULL;
    }

    if (msg.nameSize > 0) {
        msg.name = malloc(msg.nameSize + 1);
        if (msg.name == NULL) die_errno();
        memcpy(msg.name, buffer + head_size + msg.size + 1, msg.nameSize + 1);
    } else {
        msg.name = NULL;
    }

    free(buffer);

    return msg;
}

void deleteMessage(SocketMessage msg) {
    if (msg.content != NULL)
        free(msg.content);
    if (msg.name != NULL)
        free(msg.name);
}

void handleINT(int signo) {
    exit(0);
}

void cleanup(void) {
    sendEmptyMessage(UNREGISTER);
    unlink(name);
    shutdown(sock, SHUT_RDWR);
    close(sock);
}