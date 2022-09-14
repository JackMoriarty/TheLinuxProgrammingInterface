/**
 * @file connect_socket_c.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 连接socket测试c
 * @version 0.1
 * @date 2019-07-13
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _POSIX_C_SOURCE 201112L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define HOST_NAME   "127.0.0.1"
#define HOST_PORT   "5100"
#define BUF_SIZE    1024

int main(int argc, char *argv[])
{
    int cfd;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char buf[BUF_SIZE];
    socklen_t addrlen;
    ssize_t num_read;
    struct sockaddr_storage client_addr;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addr_str[ADDRSTRLEN];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s server-port\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    /* 获取客户端地址信息 */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0) {
        perror("Client gets self addr failed");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;
        if (bind(cfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        
        close(cfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    /* 获取服务器地址信息 */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    if (getaddrinfo(HOST_NAME, HOST_PORT, &hints, &result) != 0) {
        perror("Client gets server addr failed");
        exit(EXIT_FAILURE);
    }

    strcpy(buf, "Hello world");

    for (rp = result; rp != NULL; rp = rp->ai_next)
        if (sendto(cfd, buf, strlen(buf), 0, result->ai_addr, result->ai_addrlen) == -1)
            continue;
        else
            break;
    if (rp == NULL) {
        fprintf(stderr, "Could not connect to server\n");
        exit(EXIT_FAILURE);
    }

    printf("send success\n");

    freeaddrinfo(result);

    exit(EXIT_SUCCESS);
}