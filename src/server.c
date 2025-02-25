#include "../include/server.h"

typedef void (*socket_handler)(Server *server, int port);

static void read_request(Server *server) {
    char *data = (char*)malloc(BUFFER_SIZE * sizeof(char));
    server->client_sock = accept(server->server_sock, NULL, NULL);
    printf("%d\n", server->client_sock);
    if (server->client_sock != -1) {
        read(server->client_sock, data, BUFFER_SIZE * sizeof(char));
    }
}

void init_tcp_server(Server* server, int port) {
    server->server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server->sock_addr.sin_family = AF_INET;
    server->sock_addr.sin_port = htons(port);
    server->sock_addr.sin_addr.s_addr = INADDR_ANY;
    printf("Init : %d\n", port);

    if (bind(server->server_sock, (struct sockaddr*)&server->sock_addr, sizeof(server->sock_addr)) < 0) {
        perror("bind() failed for TCP");
        close(server->server_sock);
    }
    else {
        printf("TCP server bound to port: %d\n", port);
    }

    if (listen(server->server_sock, 1000) < 0) {
        perror("listen() failed for TCP");
    }
    else {
        printf("listening : %d\n", port);
    }
}

void init_udp_server(Server *server, int port) {
    int opt = 1;
    setsockopt(server->server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    server->server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    server->sock_addr.sin_family = AF_INET;
    server->sock_addr.sin_port = htons(port);
    server->sock_addr.sin_addr.s_addr = INADDR_ANY;
    printf("Init : %d\n", port);

    if (bind(server->server_sock, (struct sockaddr*)&server->sock_addr, sizeof(server->sock_addr)) < 0) {
        perror("bind() failed for UDP");
        close(server->server_sock);
    }
    else {
        printf("UDP server bound to port: %d\n", port);
    }
}

static Server *start_process(int port, socket_handler handler) {
    Server *server = (Server *)malloc(sizeof(Server));
    if (!server) {
        perror("Failed to allocate memory for server");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork()\n");
        exit(1);
    }

    if (pid == 0) {
        server->pid = getpid();
        handler(server, port);
    }

    return server;
}

Config *init_config(void) {
    Config *cnfg = (Config*)malloc(sizeof(Config));

    if (access(CONFIG_FILE_PATH, F_OK) == -1) {
        printf("config file not found.\n");
    }
    else {
        FILE *file = fopen(CONFIG_FILE_PATH, "r");
        char *line = malloc(sizeof(char) * BUFFER_SIZE + 1);

        while (fgets(line ,BUFFER_SIZE * sizeof(char), file)) {
            if (strstr(line, CONFIG_CNN_STR_KEY)) {
                char *tkn = strtok(line, ":");

                while (tkn != NULL && tkn[0] != '\0') {
                    if (!strstr(tkn, CONFIG_CNN_STR_KEY)) {
                        cnfg->cnn_str = tkn;
                    }

                    tkn = strtok(NULL, ":");
                }

                free(tkn);
                tkn = NULL;
            }
            else if (strstr(line, CONFIG_TCP_PORT_KEY)) {
                char *tkn = strtok(line, ":");

                while (tkn != NULL && tkn[0] != '\0') {
                    if (!strstr(tkn, CONFIG_TCP_PORT_KEY)) {
                        cnfg->tcp_port = atoi(tkn);
                    }

                    tkn = strtok(NULL, ":");
                }

                free(tkn);
                tkn = NULL;
            }
            else if (strstr(line, CONFIG_UDP_PORT_KEY)) {
                char *tkn = strtok(line, ":");

                while (tkn != NULL && tkn[0] != '\0') {
                    if (!strstr(tkn, CONFIG_UDP_PORT_KEY)) {
                        cnfg->udp_port = atoi(tkn);
                    }

                    tkn = strtok(NULL, ":");
                }

                free(tkn);
                tkn = NULL;
            }
        }

        //printf("config.json found.\n");
    }

    return cnfg;
}
// TODO : tcp or udp server trying to start again after it's started...
//        fix it and keep them alive...
//        cominicate between them...
void init_server(Config *cnfg) {
    Server *udp_server = start_process(cnfg->udp_port, init_udp_server);
    Server *tcp_server = start_process(cnfg->tcp_port, init_tcp_server);
    while (udp_server && tcp_server) {
        read_request(&tcp_server);
    }
}
