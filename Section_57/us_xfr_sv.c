/**
 * @file us_xfr_sv.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief Unix domain 流 socket 服务器端
 * @version 0.1
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "us_xfr.h"

#define BACKLOG     5

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path + 1, SV_SOCK_PATH, sizeof(addr.sun_path) - 2);
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
            if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                perror("partial/failed write");
                exit(EXIT_FAILURE);
            }

        if (numRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (close(cfd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}