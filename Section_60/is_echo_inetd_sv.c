/**
 * @file is_echo_inetd_sv.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief inetd 调用的TCP echo服务器
 * @version 0.1
 * @date 2019-07-14
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include "inet_sockets.h"

#define SERVICE     "echo"
#define BUF_SIZE    4096

/**
 * @brief 处理客户端请求
 * 
 * @param cfd 连接套接字
*/
static void handleRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, numRead) != numRead) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1) {
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    ssize_t numRead;
    int lfd, cfd;

    if (argc == 2 && strcmp(argv[1], "-i") == 0) {
        cfd = STDIN_FILENO;
    } else {
        lfd = inetListen(SERVICE, 10, NULL);
        if (lfd == -1) {
            syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        cfd = accept(lfd, NULL, NULL);  /* 等待连接 */
        if (cfd == -1) {
            syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    handleRequest(cfd);
    close(cfd);

    exit(EXIT_SUCCESS);
}