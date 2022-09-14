/**
 * @file fifo_server.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 服务器代码
 * @version 0.1
 * @date 2019-06-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "fifo_sercli.h"

/**
 * @brief SIGINT和SIGTERM 信号处理函数
 * 
 * @param sig 
*/
static void handler(int sig)
{
    // printf("Hello world\n");
    unlink(SERVER_FIFO);
    _exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int server_fd, dummy_fd, client_fd;
    char client_fifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seq_num = 0;
    int file_fd;
    int read_size;
    int flags;

    umask(0);
    /* 创建服务器fifo */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST) {
        fprintf(stderr, "mkfifo %s: %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, handler) == SIG_ERR) {
        perror("signal SIGTERM");
        exit(EXIT_FAILURE);
    }

    server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        fprintf(stderr, "open %s: %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* 额外打开一个写描述符, 防止读到文件末尾 */
    dummy_fd = open(SERVER_FIFO, O_WRONLY);
    if (dummy_fd == -1) {
        fprintf(stderr, "open %s: %s\n", SERVER_FIFO, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* 忽略SIGPIPE, 防止服务器向客户端写时可能出现的问题导致的程序退出 */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal SIGPIPE");
        exit(EXIT_FAILURE);
    }

    file_fd = open("backup", O_RDWR | O_CREAT | O_EXCL | O_SYNC, S_IRUSR | S_IWUSR);
    if (file_fd == -1) {
        file_fd = open("backup", O_RDWR | O_SYNC);
        if (file_fd == -1) {
            fprintf(stderr, "open backup failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    read_size = read(file_fd, &seq_num, sizeof(int));
    if (read_size == -1) {
        fprintf(stderr, "read backup file failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (read_size == 0) {
        seq_num = 0;
    }


    /* 读取请求并响应 */
    while(1) {
        if (read(server_fd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            /* 出错, 丢弃请求 */
            fprintf(stderr, "Error reading request; discarding\n");
            continue;
        }

        /* 打开客户端传递过来的FIFO */
        snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long)req.pid);
        client_fd = open(client_fifo, O_WRONLY | O_NONBLOCK);
        if (client_fd == -1) {
            /* 打开失败, 丢弃请求 */
            fprintf(stderr, "open client fifo %s failed\n", client_fifo);
            continue;
        }
        /* 取消O_NONBLOCK标记 */
        flags = fcntl(client_fd, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(client_fd, F_SETFL, flags);

        /* 响应请求 */
        resp.seq_num = seq_num;
        if (write(client_fd, &resp, sizeof(struct response))
                != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", client_fifo);
        if (close(client_fd) == -1)
            perror("close");
        
        seq_num += req.seq_len;
        if (write(file_fd, &seq_num, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "failed to write backup: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (lseek(file_fd, 0, SEEK_SET) == -1) {
            fprintf(stderr, "lseek backup failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}