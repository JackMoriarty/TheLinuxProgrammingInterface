/**
 * @file p_c_pipe.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 父子进程利用管道传递信息
 * @version 0.1
 * @date 2019-06-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "my_pipe.h"

int main(int argc, char *argv[])
{
    int pipe_fd1[2];
    int pipe_fd2[2];
    char recv_mesg[1024];
    int mesg_length;
    int send_length;
    int i;

    if (argc != 2 || !strcmp(argv[1], "--help")) {
        fprintf(stderr, "Usage: %s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 打开两个管道 */
    if (my_pipe(pipe_fd1) == -1) {
        perror("open pipe 1 failed");
        exit(EXIT_FAILURE);
    }
    if (my_pipe(pipe_fd2) == -1) {
        perror("open pipe 2 failed");
        exit(EXIT_FAILURE);
    }

    /* 创建子进程 */
    switch (fork()) {
    case -1:
        perror("create child process failed");
        exit(EXIT_FAILURE);
    
    case 0: /* 子进程 */
        /* 关闭pipe_fd1的写端和pipe_fd2的读端 */
        if (close(pipe_fd1[1]) == -1) {
            perror("child close pipe_fd1 write port failed");
            _exit(EXIT_FAILURE);
        }
        if (close(pipe_fd2[0]) == -1) {
            perror("child close pipe_fd2 read port failed");
            _exit(EXIT_FAILURE);
        }

        /* 读取数据 */
        mesg_length = read(pipe_fd1[0], recv_mesg, 1024);
        if (mesg_length == -1) {
            perror("child read failed");
            exit(EXIT_FAILURE);
        } else if (mesg_length == 0) {
            fprintf(stderr, "child recv no mesg\n");
            _exit(EXIT_FAILURE);
        }
        /* 将收到的数据转化为大写 */
        for (i = 0; i < mesg_length; i++) {
            if (recv_mesg[i] > 'a' && recv_mesg[i] < 'z')
                recv_mesg[i] -= ('a' - 'A');
        }

        /* 写回数据 */
        send_length = write(pipe_fd2[1], recv_mesg, mesg_length);
        if (send_length != mesg_length) {
            perror("child write failed");
            _exit(EXIT_FAILURE);
        }

        /* 关闭管道 */
        if (close(pipe_fd1[0]) == -1) {
            perror("close pipe_fd1 read port failed");
            _exit(EXIT_FAILURE);
        }
        if (close(pipe_fd2[1]) == -1) {
            perror("close pipe_fd2 write port failed");
            _exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);
    
    default: /* 父进程 */
        /* 关闭pipe_fd1的读端和pipe_fd2的写端 */
        if (close(pipe_fd1[0]) == -1) {
            perror("parent close pipe_fd1 read port failed");
            exit(EXIT_FAILURE);
        }
        if (close(pipe_fd2[1]) == -1) {
            perror("parent close pipe_fd2 write port failed");
            exit(EXIT_FAILURE);
        }

        /* 向管道中写数据 */
        mesg_length = strlen(argv[1]);
        send_length = write(pipe_fd1[1], argv[1], mesg_length);
        if (send_length != mesg_length) {
            fprintf(stderr, "parent write failed\n");
            exit(EXIT_FAILURE);
        }
        
        mesg_length = read(pipe_fd2[0], recv_mesg, 1024);
        if (mesg_length == -1) {
            perror("parent read pipe_fd2 failed");
            exit(EXIT_FAILURE);
        } else if (mesg_length == 0) {
            fprintf(stderr, "parent get nothing\n");
            exit(EXIT_FAILURE);
        }
        printf("parent get mesg: %s\n", recv_mesg);
        /* 关闭管道 */
        if (close(pipe_fd1[1]) == -1) {
            perror("close pipe_fd1 write port failed");
            exit(EXIT_FAILURE);
        }
        if (close(pipe_fd2[0]) == -1) {
            perror("close pipe_fd2 read port failed");
            exit(EXIT_FAILURE);
        }
        
        /* 等待子进程退出 */
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}