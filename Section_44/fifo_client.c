/**
 * @file fifo_client.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 客户端代码
 * @version 0.1
 * @date 2019-06-17
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "fifo_sercli.h"

static char client_fifo[CLIENT_FIFO_NAME_LEN];

static void remove_fifo(void)
{
    unlink(client_fifo);
}

int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [seq-len...]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* 创建客户端FIFO */
    umask(0);
    snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                    (long) getpid());
    if (mkfifo(client_fifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
                && errno != EEXIST) {
        fprintf(stderr, "mkfifio %s: %s\n", client_fifo, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_fifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    /* 构建请求信息, 打开服务器FIFO并发送请求 */
    req.pid = getpid();
    req.seq_len = (argc > 1) ? atoi(argv[1]) : 1;

    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        fprintf(stderr, "open %s: %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (write(server_fd, &req, sizeof(struct request))
            != sizeof(struct request)) {
        fprintf(stderr, "Can't write to server");
        exit(EXIT_FAILURE);
    }

    /* 打开客户端FIFO, 读取并展示信息 */
    client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1) {
        fprintf(stderr, "open %s\n", client_fifo);
        exit(EXIT_FAILURE);
    }

    if (read(client_fd, &resp, sizeof(struct response))
            != sizeof(struct response)) {
        fprintf(stderr, "Can't read response from server");
    }

    printf("%d\n", resp.seq_num);
    
    exit(EXIT_SUCCESS);
}