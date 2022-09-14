/**
 * @file my_telnet_server.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief telnet 风格客户端
 * @version 0.1
 * @date 2019-07-21
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <termios.h>
#include "inet_sockets.h"
#include "pty_fork.h"

#define SERVICE     "5050"
#define BUF_SIZE    1024
#define MAX_SNAME   1000

/**
 * @brief 处理客户端请求
 * 
 * @param cfd 客户端socket描述符
*/
static void handle_request(int cfd)
{
    int masterFd;
    pid_t childPid;
    char slaveName[MAX_SNAME];
    char buf[BUF_SIZE];
    char *shell;
    fd_set inFds;
    int numRead, numWrite;
    int maxfd;
    
    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, NULL, NULL);
    if (childPid  == -1) {
        perror("ptyFork");
        exit(EXIT_FAILURE);
    }

    if (childPid == 0) {
        shell = getenv("SHELL");
        if (shell == NULL || *shell == '\0')
            shell = "/bin/sh";
        
        execlp(shell, shell, (char *) NULL);
        // execlp("/usr/bin/login", "login", "-p", "-H", (char *) NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    /* 父进程 */
    maxfd = (cfd > masterFd) ? cfd : masterFd;
    for (;;) {
        FD_ZERO(&inFds);
        FD_SET(masterFd, &inFds);
        FD_SET(cfd, &inFds);

        if (select(maxfd + 1, &inFds, NULL, NULL, NULL) == -1) {
            perror("SELECT");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(masterFd, &inFds)) {
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(cfd, &buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (cfd)\n");
        }

        if (FD_ISSET(cfd, &inFds)) {
            numRead = read(cfd, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(masterFd, buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (masterFd)\n");
        }
    }
}

int main(int argc, char *argv[])
{
    int lfd, cfd;

    lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        perror("Could not create server socket");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
            perror("Failure in accept");
            exit(EXIT_FAILURE);
        }

        switch (fork()) {
        case -1:
            perror("Create child process failed");
            close(cfd);

        case 0:
            close(lfd);
            handle_request(cfd);
            /* 关闭服务器套接字*/
            close(cfd);
            _exit(EXIT_SUCCESS);
        
        default:
            close(cfd);
        }
    }

    exit(EXIT_SUCCESS);
}