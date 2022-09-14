/**
 * @file tcp_udp_echo.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 同时支持tcp和udp的echo服务器
 * @version 0.1
 * @date 2019-07-18
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#define TCP_ECHO_PORT   "5050"
#define UDP_ECHO_PORT   "5051"

void handler_request(int lfd)
{
#define BUF_SIZE    1024
    char buf[BUF_SIZE], *ptr;
    int num_read, num_write, n;
    struct sockaddr_storage ss;
    socklen_t addrlen;
    
    addrlen = sizeof(struct sockaddr);
    num_read = recvfrom(lfd, buf, BUF_SIZE, 0, (struct sockaddr *)&ss, &addrlen);
    if (num_read == -1) {
        perror("read message");
        return;
    }

    ptr = buf;
    n = num_read;
    for (;;) {
        num_write = sendto(lfd, ptr, n, 0, (struct sockaddr *) &ss, addrlen);
        if (num_write == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("write message");
                return;
            }
        }

        if (num_write < n) {
            ptr += num_write;
            n -= num_write;
        } else {
            break;
        }
    }
    
    
}


int main(int argc, char *argv[])
{
    int tcp_fd, udp_fd, lfd, nfds;
    struct addrinfo hints, *res, *rp;
    int optval, ready;
    fd_set readfds;

    /* 创建tcp和udp socket */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(NULL, TCP_ECHO_PORT, &hints, &res) !=  0) {
        perror("tcp getaddrinfo");
        exit(EXIT_FAILURE);
    }

    optval = 1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        tcp_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (tcp_fd == -1)
            continue;

        if (tcp_fd > FD_SETSIZE) {
            fprintf(stderr, "tcp_fd is bigger than FD_SETSIZE");
            exit(EXIT_FAILURE);
        }
        
        if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
                 == -1) {
            perror("tcp setsockopt");
            exit(EXIT_FAILURE);
        }
        if (bind(tcp_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(tcp_fd);
    }

    if (rp == NULL) {
        fprintf(stderr, "TCP: Could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
    if (listen(tcp_fd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(NULL, UDP_ECHO_PORT, &hints, &res) != 0) {
        perror("udp getaddrinfo");
        exit(EXIT_FAILURE);
    }

    optval = 1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        udp_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (udp_fd == -1)
            continue;

        if (udp_fd > FD_SETSIZE) {
            fprintf(stderr, "udp_fd is bigger than FD_SETSIZE");
            exit(EXIT_FAILURE);
        }
        
        if (bind(udp_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(udp_fd);
    }

    if (rp == NULL) {
        fprintf(stderr, "UDP: Could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);

    nfds = ((tcp_fd > udp_fd) ? tcp_fd : udp_fd) + 1;
    
    for (;;) {
        FD_ZERO(&readfds);
        FD_SET(tcp_fd, &readfds);
        FD_SET(udp_fd, &readfds);

        ready = select(nfds, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select");
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(tcp_fd, &readfds)) {

            lfd = accept(tcp_fd, NULL, NULL);
            if (lfd == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(udp_fd, &readfds)) {
            lfd = udp_fd;
        }

        handler_request(lfd);
    }

    exit(EXIT_FAILURE);
}