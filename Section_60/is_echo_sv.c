/**
 * @file is_echo_sv.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 并发型TCP echo服务器
 * @version 0.1
 * @date 2019-07-13
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "become_daemon.h"
#include "inet_sockets.h"

#define SERVICE     "echo"
#define BUF_SIZE    4096
#define MAX_CONNECT_NUM     2

static unsigned connect_num = 0;

/**
 * @brief SIGCHLD 信号处置函数
 * 
 * @param sig 信号值
*/
static void grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        connect_num--;
        continue;
    }
    errno = savedErrno;
}

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
    int lfd, cfd;
    struct sigaction sa;

    if (become_daemon(0) == -1) {
        perror("become_daemon");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (;;) {
        cfd = accept(lfd, NULL, NULL);  /* 等待连接 */
        if (cfd == -1) {
            syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (connect_num >= MAX_CONNECT_NUM) {
            close(cfd);
            continue;
        }

        /* 创建子进程处理客户端请求 */
        switch (fork()) {
        case -1:
            syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
            close(cfd);
            break;
        
        case 0:
            close(lfd);
            handleRequest(cfd);
            _exit(EXIT_FAILURE);

        default:
            /* 暂时阻止信号的传递 */
            sigaddset(&sa.sa_mask, SIGCHLD);
            if (sigaction(SIGCHLD, &sa, NULL) == -1) {
                syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            connect_num++;
            /* 解除对信号的阻塞 */
            sigemptyset(&sa.sa_mask);
            if (sigaction(SIGCHLD, &sa, NULL) == -1) {
                syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            close(cfd);
            break;
        }
    }
}
