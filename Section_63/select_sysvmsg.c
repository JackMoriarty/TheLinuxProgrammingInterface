/**
 * @file select_sysvmsg.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 利用管道机制实现select函数对sysv消息队列的监视
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

#define KEY 0x1234
#define BUF_SIZE    1024

static int pipe_fd[2];
static pid_t child_pid;
static int msg_id;

struct mymsg {
    long mtype;
    char mtext[BUF_SIZE];
};

void child_func()
{
    struct mymsg buf;
    char *buf_p;
    ssize_t num_read, num_write;
    int n;

    /* 关闭管道读端 */
    close(pipe_fd[0]);

    /* 将消息队列中的消息写到管道中 */
    for (;;) {
        num_read = msgrcv(msg_id, &buf, BUF_SIZE, 0, 0);
        if (num_read == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        }
        n = sizeof(struct mymsg);
        buf_p = (char *)&buf;
        for (num_write = 0; num_write < n;) {
            num_write = write(pipe_fd[1], buf_p, n);
            if (num_write == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }

            buf_p += num_write;
            n -= num_write;
        }

    }
}

void parent_func()
{
    fd_set readfds;
    int ready;
    int nfds;
    char buf[1024];
    int num_read;
    int flags;

    /* 关闭管道写端 */
    close(pipe_fd[1]);

    nfds = ((STDIN_FILENO > pipe_fd[0]) ? STDIN_FILENO : pipe_fd[0]) + 1;

    /* 设置两者为非阻塞IO */
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags == -1) {
        kill(child_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1) {
        kill(child_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }
    flags = fcntl(pipe_fd[0], F_GETFL);
    if (flags == -1) {
        kill(child_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(pipe_fd[0], F_SETFL, flags) == -1) {
        kill(child_pid, SIGTERM);
        exit(EXIT_FAILURE);
    }

    /* 监视 */
    for (;;) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(pipe_fd[0], &readfds);

        ready = select(nfds, &readfds, NULL, NULL, NULL);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select");
                /* 子进程退出 */
                kill(child_pid, SIGTERM);
                exit(EXIT_FAILURE);
            }
        }

        printf("%s:%s\n", (FD_ISSET(STDIN_FILENO, &readfds)) ? "stdin" : "",
                            (FD_ISSET(pipe_fd[0], &readfds)) ? "sysvmsg" : "");

        /* 消耗数据 */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            for (;;) {
                num_read = read(STDIN_FILENO, buf, BUF_SIZE);
                if (num_read == -1 && errno == EAGAIN)
                    break;
            }
        }
        if (FD_ISSET(pipe_fd[0], &readfds)) {
            for (;;) {
                num_read = read(pipe_fd[0], buf, BUF_SIZE);
                if (num_read == -1 && errno == EAGAIN)
                    break;
            }
        }
    }
}

void sysvmsg_rm()
{
    msgctl(msg_id, IPC_RMID, NULL);
}

int main(int argc, char *argv[])
{
    struct termios origTermios;

    msg_id = msgget(KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (msg_id == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    if (atexit(sysvmsg_rm) != 0) {
        sysvmsg_rm();
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* 创建子进程将消息队列中的消息移动到管道中 */
    switch (child_pid = fork()) {
    case -1:
        perror("fork()");
        exit(EXIT_FAILURE);

    case 0: /* 子进程 */
        child_func();
        /* 不会到达这里 */
        exit(EXIT_FAILURE);

    default: /* 父进程 */
        parent_func();
        /* 不会到达这里 */
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}