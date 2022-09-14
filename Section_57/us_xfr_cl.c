/**
 * @file us_xfr_cl.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief UNIX domain 流 socket 客户端
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

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd;
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

    if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
        if (write(sfd, buf, numRead) != numRead) {
            perror("partial/failed write");
            exit(EXIT_FAILURE);
        }

    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}