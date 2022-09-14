/**
 * @file epoll.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief epoll 编程接口
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int epoll_fd;
    struct epoll_event ev;
    int ready;

    epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ready = epoll_wait(epoll_fd, &ev, 1, -1);
    if (ready == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }

    printf("ready: %d\n", ready);

    exit(EXIT_FAILURE);
}