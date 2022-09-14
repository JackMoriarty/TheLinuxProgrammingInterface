/**
 * @file connect_socket_b.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief unix socket连接测试b
 * @version 0.1
 * @date 2019-07-13
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define LOCAL_PATH  "/tmp/b"
#define HOST_PATH   "/tmp/a"
#define BUF_SIZE    1024
#define BUF_STRING  "I am b\n"

int main(int argc, char *argv[]) {

    struct sockaddr_un claddr, svaddr;
    int cfd;
    char buf[BUF_SIZE];
    ssize_t num_send;

    /* 创建socket */
    cfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (cfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* 绑定本地地址 */
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    strncpy(claddr.sun_path, LOCAL_PATH, sizeof(claddr.sun_path) - 1);
    if (bind(cfd, (struct sockaddr *) &claddr, 
                sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Press any key to send message...");
    fflush(stdout);
    getchar();
    /* 向远端地址发送数据 */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, HOST_PATH, sizeof(svaddr.sun_path) - 1);
    memset(buf, 0, BUF_SIZE);
    strncpy(buf, BUF_STRING, BUF_SIZE - 1);
    num_send = sendto(cfd, buf, BUF_SIZE, 0, (struct sockaddr *) &svaddr,
                        sizeof(struct sockaddr_un));
    if (num_send == -1)
        perror("sendto");
    else 
        printf("send %ld bytes message\n", (long) num_send);

    close(cfd);
    remove(LOCAL_PATH);
    exit(EXIT_SUCCESS);
}