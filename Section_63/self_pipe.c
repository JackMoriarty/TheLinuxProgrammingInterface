/**
 * @file self_pipe.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 采用自管道技术
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static int pfd[2];

static void handler(int sig)
{
    int savedErrno;

    savedErrno = errno;
    if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    errno = savedErrno;
}

int main(int argc, char *argv[])
{
    struct pollfd fds;
    int nfds;
    int ready;
    int flags;
    struct sigaction sa;
    char ch;

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    fds.fd = pfd[0];
    fds.events = POLLIN;
    nfds = 1;

    flags = fcntl(pfd[0], F_GETFL);
    if (flags == -1) {
        perror("fcntl-F_GETFL read end");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(pfd[0], F_SETFL) == -1) {
        perror("fcntl-F_SETFL read end");
        exit(EXIT_FAILURE);
    }

    flags = fcntl(pfd[1], F_GETFL);
    if (flags == -1) {
        perror("fcntl-F_GETFL write end");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(pfd[1], F_SETFL) == -1) {
        perror("fcntl-F_SETFL");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while ((ready = poll(&fds, nfds, -1)) == -1 &&
            errno == EINTR)
        continue;
    if (ready == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    if (fds.revents & POLLIN) {
        printf("A signal was caught\n");

        for (;;) {
            if (read(pfd[0], &ch, 1) == -1) {
                if (errno == EAGAIN) {
                    break;
                } else {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                    
            }
        }
    }
}