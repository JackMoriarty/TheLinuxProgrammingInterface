/**
 * @file is_shell_cl.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief shell命令客户端
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include "inet_sockets.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVICE     "ssh"
#define BUF_SIZE    10240

int main(int argc, char *argv[])
{
    int cfd;
    char buf[BUF_SIZE];
    ssize_t num_read;
    ssize_t num_write;
    ssize_t num_socket_read;
    

    cfd = inetConnect(SERVER_ADDR, SERVICE, SOCK_STREAM);
    if (cfd == -1) {
        perror("Failed to connect to the server");
        exit(EXIT_FAILURE);
    }


    printf("ssh:>");
    fflush(stdout);
    
    /* 忽略部分读写问题 */
    num_read = read(STDIN_FILENO, buf, BUF_SIZE);
    if (num_read == -1) {
        perror("Failed to read");
        exit(EXIT_FAILURE);
    } else if (num_read == 0) {
        exit(EXIT_SUCCESS);
    }

    buf[num_read - 1] = '\0';

    num_write = send(cfd, buf, num_read, 0);
    if (num_write != num_read) {
        perror("Failed to write");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        num_socket_read = recv(cfd, buf, BUF_SIZE, 0);
        if (num_socket_read == -1) {
            exit(EXIT_FAILURE);
        } else if(num_socket_read == 0) {
            exit(EXIT_SUCCESS);
        }

        write(STDOUT_FILENO, buf, num_socket_read);
    }
    exit(EXIT_SUCCESS);
}