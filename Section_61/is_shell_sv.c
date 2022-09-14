/**
 * @file is_shell_sv.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief shell 命令服务器端
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include "inet_sockets.h"
#include "my_system.h"

#define SERVICE     "ssh"
#define BUF_SIZE    1024

/**
 * @brief 处理客户端请求
 * 
 * @param cfd 客户端socket描述符
*/
static void handle_request(int cfd)
{
    char buf[BUF_SIZE];
    int num_read;

    /* 输出重定向 */
    if (dup2(cfd, STDOUT_FILENO) == -1) {
        perror("dup cfd for stdout");
        exit(EXIT_FAILURE);
    }
    if (dup2(cfd, STDERR_FILENO) == -1) {
        /* may not print message */
        perror("dup cfd for stderr");
        exit(EXIT_FAILURE);
    }

    num_read = recv(cfd, buf, BUF_SIZE, 0);
    if(num_read == -1) {
        exit(EXIT_FAILURE);
    }
    
    buf[BUF_SIZE - 1] = '\0';
    my_system(buf);
    printf("Moriarty\n");
    while(1);
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