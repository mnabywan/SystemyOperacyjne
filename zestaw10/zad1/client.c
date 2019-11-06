#include "utils.h"

int sock;
char *name;



void init(char *n, char *variant, char *address) {
    // register atexit
    if (atexit(cleanup) == -1) {
        printf(" ");
        exit(1);
    }
    // register int handler
    signal(SIGINT, handleINT);


    name = n;
    if (strcmp("WEB", variant) == 0) {
        strtok(address, ":");
        char *port = strtok(NULL, ":");
        if (port == NULL){
            printf("Port is null");
            exit(1);
        } 

        uint32_t in_addr = inet_addr(address);
        if (in_addr == INADDR_NONE){
            printf("Invalid address");
            exit(1);
        } 

        uint16_t port_num = (uint16_t) atoi(port);
        if (port_num < 1024){
            printf("Invalid port number");
            exit(1);
        }

        struct sockaddr_in web_addr;
        memset(&web_addr, 0, sizeof(struct sockaddr_in));

        web_addr.sin_family = AF_INET;
        web_addr.sin_addr.s_addr = in_addr;
        web_addr.sin_port = htons(port_num);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            printf("Error in socket");
            exit(1);
        }

        if (connect(sock, (const struct sockaddr *) &web_addr, sizeof(web_addr)) == -1){
            printf("Error in connect");
            exit(1);
        }
    } else if (strcmp("UNIX", variant) == 0) {
        char *un_path = address;

        if (strlen(un_path) < 1 || strlen(un_path) > UNIX_PATH_MAX){
            printf("Invalid unix socket path");
            exit(1);
        }
        struct sockaddr_un un_addr;
        un_addr.sun_family = AF_UNIX;
        snprintf(un_addr.sun_path, UNIX_PATH_MAX, "%s", un_path);

        if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
            printf("Error in socket");
            exit(1);
        }
        if (connect(sock, (const struct sockaddr *) &un_addr, sizeof(un_addr)) == -1){
            printf("Error in connect");
            exit(1);
        }
    } else {
        printf("Unknown variant");
    }

    sendEmptyMessage(REGISTER);
}

void sendMessage(SocketMessage * msg) {
    write(sock, &msg->type, sizeof(msg->type));
    write(sock, &msg->size, sizeof(msg->size));
    write(sock, &msg->nameSize, sizeof(msg->nameSize));
    write(sock, &msg->id, sizeof(msg->id));
    if (msg->size > 0) write(sock, msg->content, msg->size);
    if (msg->nameSize > 0) write(sock, msg->name, msg->nameSize);
}

void sendEmptyMessage(SocketMessageType type) {
    SocketMessage msg = {type, 0, strlen(name) + 1, 0, NULL, name};
    sendMessage(&msg);
}

void sendDoneMessage(int id, char *content) {
    SocketMessage msg = {WORK_DONE, strlen(content) + 1, strlen(name) + 1, id, content, name};
    sendMessage(&msg);
}

SocketMessage getMessage(void) {
    SocketMessage msg;
    if (read(sock, &msg.type, sizeof(msg.type)) != sizeof(msg.type)){
        printf("Uknown message from server");
        exit(1);
    }
    if (read(sock, &msg.size, sizeof(msg.size)) != sizeof(msg.size)){
        printf("Uknown message from server");
        exit(1);
    }
    if (read(sock, &msg.nameSize, sizeof(msg.nameSize)) != sizeof(msg.nameSize)){
        printf("Uknown message from server");
        exit(1);
    }
    if (read(sock, &msg.id, sizeof(msg.id)) != sizeof(msg.id)){
        printf("Uknown message from server");
        exit(1);       
    }
    if (msg.size > 0) {
        msg.content = malloc(msg.size + 1);
        if (msg.content == NULL){
            printf("Message is null");
            exit(1);
        }
        if (read(sock, msg.content, msg.size) != msg.size) {
            printf("Uknown message from server");
            exit(1);
        }
    } else {
        msg.content = NULL;
    }
    if (msg.nameSize > 0) {
        msg.name = malloc(msg.nameSize + 1);
        if (msg.name == NULL){
            printf("Bad message");
            exit(1);
        } 
        if (read(sock, msg.name, msg.nameSize) != msg.nameSize) {
            printf("Uknown message from server");
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
    sendEmptyMessage(UNREGISTER);
    shutdown(sock, SHUT_RDWR);
    close(sock);
}


int main(int argc, char *argv[]) {
    if (argc != 4){
        printf("Bad args");
        exit(1);
    }
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
                printf("Name is already taken");
                exit(1);
            
            case FULL:
                printf("Server is full");
                exit(1);
            case WORK: {
                puts("Doing work...");
                char *buffer = malloc(100 + 2 * msg.size);
                if (buffer == NULL){
                    printf(" ");
                    exit(1);
                } 
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
