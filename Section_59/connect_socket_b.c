/**
 * @file connect_socket_b.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 连接socket测试b
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

#define PORT_NUM    "5050"
#define BUF_SIZE    1024

int main(int argc, char *argv[])
{
    int sfd;
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

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        
        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    for (;;) {
        addrlen = sizeof(struct sockaddr_storage);
        num_read = recvfrom(sfd, buf, BUF_SIZE, 0, 
                    (struct sockaddr *)&client_addr, &addrlen);
        if (num_read == -1) {
            fprintf(stderr, "Failed to recv message\n");
            continue;
        }
        if (getnameinfo((struct sockaddr *) &client_addr, addrlen, host, 
                        NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addr_str, ADDRSTRLEN, "[%s:%s]", host, service);
        else
            snprintf(addr_str, ADDRSTRLEN, "[UNKNOWN]");
        
        printf("Receieve %ld bytes message from %s\n", (long)num_read, addr_str);
    }

    exit(EXIT_SUCCESS);
}