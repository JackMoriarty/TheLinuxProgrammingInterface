/**
 * @file my_telnet_client.c
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
#include <sys/socket.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include "inet_sockets.h"
#include "tty_functions.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVICE     "5050"
#define BUF_SIZE    10240

struct termios ttyOrig;

static void ttyReset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int cfd;
    char buf[BUF_SIZE];
    ssize_t num_read;
    ssize_t num_write;
    ssize_t num_socket_read;
    fd_set infds;
    

    cfd = inetConnect(SERVER_ADDR, SERVICE, SOCK_STREAM);
    if (cfd == -1) {
        perror("Failed to connect to the server");
        exit(EXIT_FAILURE);
    }

    ttySetRaw(STDIN_FILENO, &ttyOrig);

    if (atexit(ttyReset) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        FD_ZERO(&infds);
        FD_SET(STDIN_FILENO, &infds);
        FD_SET(cfd, &infds);

        if (select(cfd + 1, &infds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &infds)) {
            num_read = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_read <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(cfd, buf, num_read) != num_read)
                fprintf(stderr, "partial/failed write (cfd)\n");
        }

        if (FD_ISSET(cfd, &infds)) {
            num_read = read(cfd, buf, BUF_SIZE);
            if (num_read <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(STDOUT_FILENO, buf, num_read) != num_read)
                fprintf(stderr, "partial/failed write (cfd)\n");
        }
    }

}