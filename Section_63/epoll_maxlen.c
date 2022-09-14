/**
 * @file epoll_maxlen.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 当待返回的文件描述符个数大于等待提供的大小, epoll_wait的特性
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>



int main(int argc, char *argv[])
{
    int pfd[3][2];
    int epoll_fd;
    struct epoll_event ev[3];
    int ready;
    struct epoll_event evlist;
    int num_write;
    char ch = 'x';
    int i;

    if (pipe(pfd[0]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pfd[1]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pfd[2]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create(3);
    if (epoll_fd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev[0].data.fd = pfd[0][0];
    ev[0].events = EPOLLIN;
    ev[1].data.fd = pfd[1][0];
    ev[1].events = EPOLLIN;
    ev[2].data.fd = pfd[2][0];
    ev[2].events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pfd[0][0], &ev[0]) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pfd[1][0], &ev[1]) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pfd[2][0], &ev[2]) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }  

    num_write = write(pfd[0][1], &ch, sizeof(char));
    if (num_write == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    num_write = write(pfd[1][1], &ch, sizeof(char));
    if (num_write == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    num_write = write(pfd[2][1], &ch, sizeof(char));
    if (num_write == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 10; i++) {
        ready = epoll_wait(epoll_fd, &evlist, 1, -1);
        if (ready == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        if (evlist.events & EPOLLIN)
            printf("fd: %d\n", evlist.data.fd);
        else 
            printf("Not EPOLLIN\n"); 
    }
    
    exit(EXIT_SUCCESS);
}