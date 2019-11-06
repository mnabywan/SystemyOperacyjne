#include "utils.h"

int un_sock;
int web_sock;
int epoll;
char *un_path;

uint64_t id;

pthread_mutex_t clientMutex = PTHREAD_MUTEX_INITIALIZER;
Client clients[MAX_CLIENTS];

pthread_t pinger;
pthread_t commander;


void init(char *port, char *unix_path) {
    atexit(cleanup);
    signal(SIGINT, handleINT);

    uint16_t port_num = (uint16_t) atoi(port);
    if (port_num < 1024){
        printf("Wrong port num");
        exit(1);
    }
    un_path = unix_path;
    if (strlen(un_path) < 1 || strlen(un_path) > UNIX_PATH_MAX){
        printf("Wrong socket path");
        exit(1);
    }
    struct sockaddr_in web_addr;
    memset(&web_addr, 0, sizeof(struct sockaddr_in));
    web_addr.sin_family = AF_INET;
    web_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    web_addr.sin_port = htons(port_num);

    if ((web_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Cannot create socket");
        exit(1);
    }

    if (bind(web_sock, (const struct sockaddr *) &web_addr, sizeof(web_addr))){
        printf("Cannot bind socket");
        exit(1);
    }

    if (listen(web_sock, 64) == -1){
        printf("Cannot listen");
        exit(1);
    }
    
    struct sockaddr_un un_addr;
    un_addr.sun_family = AF_UNIX;

    sprintf(un_addr.sun_path, "%s", un_path);

    if ((un_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        printf("error socket");
        exit(1);
    }

    if (bind(un_sock, (const struct sockaddr *) &un_addr, sizeof(un_addr))){
        printf("error bind");
        exit(1);
    }

    if (listen(un_sock, MAX_CLIENTS) == -1){
        printf("error listen");
        exit(1);
    }

    // init epoll
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;

    if ((epoll = epoll_create1(0)) == -1){
        printf("error epoll");
        exit(1);
    }
    
    event.data.fd = -web_sock;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, web_sock, &event) == -1){
        printf("error epoll");
        exit(1);
    }

    event.data.fd = -un_sock;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, un_sock, &event) == -1){
        printf("error epoll");
        exit(1);
    }   
    
    // start threads
    if (pthread_create(&commander, NULL, terminalHandler, NULL) != 0){
        printf("error creating thread");
        exit(1);
    }
    if (pthread_create(&pinger, NULL, pingerHandler, NULL) != 0){
        printf("error creating thread");
        exit(1);
    }   
}

void *terminalHandler(void *params) {
    char buffer[1024];
    while (1) {
        int min_i = MAX_CLIENTS;
        int min = 1000000;

        // read command
        scanf("%1023s", buffer);

        // open file
        FILE *file = fopen(buffer, "r");
        if (file == NULL) {
            continue;
        }
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        char *file_buff = malloc(size + 1);
        if (file_buff == NULL) {
            continue;
        }

        file_buff[size] = '\0';

        if (fread(file_buff, 1, size, file) != size) {
            fprintf(stderr, "Could not read file\n");
            free(file_buff);
            continue;
        }

        fclose(file);

        // send request
        pthread_mutex_lock(&clientMutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].fd) continue;
            if (min > clients[i].working) {
                min_i = i;
                min = clients[i].working;
            }
        }

        if (min_i < MAX_CLIENTS) {
            SocketMessage msg = {WORK, strlen(file_buff) + 1, 0, ++id, file_buff, NULL};
            printf("JOB %lu SEND TO %s\n", id, clients[min_i].name);
            sendMessage(clients[min_i].fd, msg);
            clients[min_i].working++;
        } else {
            fprintf(stderr, "No clients connected\n");
        }
        pthread_mutex_unlock(&clientMutex);

        free(file_buff);
    }
}

void *pingerHandler(void *params) {
    while (1) {
        pthread_mutex_lock(&clientMutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd == 0) continue;
            if (clients[i].inactive) {
                deleteClient(i);
            } else {
                clients[i].inactive = 1;
                sendEmptyMessage(clients[i].fd, PING);
            }
        }
        pthread_mutex_unlock(&clientMutex);
        sleep(10);
    }
}

void handleRegistration(int sock) {
    puts("Client registered");
    int client = accept(sock, NULL, NULL);
    if (client == -1){
        printf(" ");
        exit(1);
    } 
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = client;

    if (epoll_ctl(epoll, EPOLL_CTL_ADD, client, &event) == -1)
        printError("");
}

void handleMessage(int sock) {
    SocketMessage msg = getMessage(sock);
    pthread_mutex_lock(&clientMutex);

    switch (msg.type) {
        case REGISTER: {
            SocketMessageType reply = OK;
            int i;
            i = getClientByName(msg.name);
            if (i < MAX_CLIENTS)
                reply = NAME_TAKEN;

            for (i = 0; i < MAX_CLIENTS && clients[i].fd != 0; i++);

            if (i == MAX_CLIENTS)
                reply = FULL;

            if (reply != OK) {
                sendEmptyMessage(sock, reply);
                deleteSocket(sock);
                break;
            }

            clients[i].fd = sock;
            clients[i].name = malloc(msg.size + 1);
            if (clients[i].name == NULL) {
                printf(" ");
                exit(1);
            }
            strcpy(clients[i].name, msg.name);
            clients[i].working = 0;
            clients[i].inactive = 0;

            sendEmptyMessage(sock, OK);
            break;
        }
        case UNREGISTER: {
            int i;
            for (i = 0; i < MAX_CLIENTS && strcmp(clients[i].name, msg.name) != 0; i++);
            if (i == MAX_CLIENTS) break;
            deleteClient(i);
            break;
        }
        case WORK_DONE: {
            int i = getClientByName(msg.name);
            if (i < MAX_CLIENTS) {
                clients[i].inactive = 0;
                clients[i].working--;
            }
            printf("JOB %lu DONE BY %s:\n%s\n", msg.id, (char *) msg.name, (char *) msg.content);
            break;
        }
        case PONG: {
            int i = getClientByName(msg.name);
            if (i < MAX_CLIENTS)
                clients[i].inactive = 0;
        }
    }

    pthread_mutex_unlock(&clientMutex);

    deleteMessage(msg);
}

void deleteClient(int i) {
    deleteSocket(clients[i].fd);
    clients[i].fd = 0;
    clients[i].name = NULL;
    clients[i].working = 0;
    clients[i].inactive = 0;
}

int getClientByName(char *name) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == 0) continue;
        if (strcmp(clients[i].name, name) == 0)
            break;
    }
    return i;
}

void deleteSocket(int sock) {
    epoll_ctl(epoll, EPOLL_CTL_DEL, sock, NULL);
    shutdown(sock, SHUT_RDWR);
    close(sock);
}

void sendMessage(int sock, SocketMessage msg) {
    write(sock, &msg.type, sizeof(msg.type));
    write(sock, &msg.size, sizeof(msg.size));
    write(sock, &msg.nameSize, sizeof(msg.nameSize));
    write(sock, &msg.id, sizeof(msg.id));
    if (msg.size > 0) write(sock, msg.content, msg.size);
    if (msg.nameSize > 0) write(sock, msg.name, msg.nameSize);
}

void sendEmptyMessage(int sock, SocketMessageType reply) {
    SocketMessage msg = {reply, 0, 0, 0, NULL, NULL};
    sendMessage(sock, msg);
};

SocketMessage getMessage(int sock) {
    SocketMessage msg;
    if (read(sock, &msg.type, sizeof(msg.type)) != sizeof(msg.type)){
        printf("Uknown message from client");
        exit(1);
    }
    if (read(sock, &msg.size, sizeof(msg.size)) != sizeof(msg.size)){
        printf("Uknown message from client");
        exit(1);
    }
    if (read(sock, &msg.nameSize, sizeof(msg.nameSize)) != sizeof(msg.nameSize)){
        printf("Uknown message from client");
        exit(1);
    }
    if (read(sock, &msg.id, sizeof(msg.id)) != sizeof(msg.id)){
        printf("Uknown message from client");
        exit(1);
    }
    if (msg.size > 0) {
        msg.content = malloc(msg.size + 1);
        if (msg.content == NULL) {
            printf(" ");
            exit(1);
        }
        if (read(sock, msg.content, msg.size) != msg.size) {
            printf("Uknown message from client");
            exit(1);
        }
    } else {
        msg.content = NULL;
    }
    if (msg.nameSize > 0) {
        msg.name = malloc(msg.nameSize + 1);
        if (msg.name == NULL){
            printf(" ");
            exit(1);
        } 
        if (read(sock, msg.name, msg.nameSize) != msg.nameSize) {
            printf("Uknown message from client");
            exit(1);   
        }
    } else {
        msg.name = NULL;
    }
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
    close(web_sock);
    close(un_sock);
    unlink(un_path);
    close(epoll);
}

int main(int argc, char *argv[]) {
    if (argc != 3){
        printf("Bad args in server");
        exit(1);
    }

    init(argv[1], argv[2]);

    struct epoll_event event;
    while (1) {
        if (epoll_wait(epoll, &event, 1, -1) == -1){
            printf("Errror in epoll wait");
            exit(1);
        }

        if (event.data.fd < 0)
            handleRegistration(-event.data.fd);
        else
            handleMessage(event.data.fd);
    }
}
