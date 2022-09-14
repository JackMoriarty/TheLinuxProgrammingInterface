/**
 * @file connect_socket_a.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 连接socket测试a
 * @version 0.1
 * @date 2019-07-12
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
#include <unistd.h>
#include <netdb.h>

#define HOST_NAME   "127.0.0.1"
#define HOST_PORT   "5050"
#define PORT_NUM    "5100"
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

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;

        
        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(cfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not connect to server\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    printf("Client connect to server\n");
    
    if (send(cfd, "Hello!", strlen("Hello!"), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    } else {
        printf("send success!");
    }

    if (recv(cfd, buf, BUF_SIZE, 0) == -1) {
        perror("client recv");
        exit(EXIT_FAILURE);
    } else {
        printf("recv message");
    }

    exit(EXIT_SUCCESS);
}