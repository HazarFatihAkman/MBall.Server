#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define BUFFER_SIZE 512
#define CONFIG_FILE_PATH "config.gokturk.tech"
#define CONFIG_CNN_STR_KEY "connection_string"
#define CONFIG_TCP_PORT_KEY "tcp_port"
#define CONFIG_UDP_PORT_KEY "udp_port"

typedef struct {
    int open, pid, port, type, time; //0 = tcp | 1 = udp
} SessionHistory;

typedef struct {
    char *cnn_str;
    int tcp_port, udp_port;
} Config;

typedef struct {
    int client_sock, server_sock;
    pid_t pid;
    struct sockaddr_in sock_addr;
} Server;

Config *init_config(void);
void init_server(Config*);

#endif