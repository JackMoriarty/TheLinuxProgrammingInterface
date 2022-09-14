/**
 * @file mq_notify_sig.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 通过信号接收消息通知
 * @version 0.1
 * @date 2019-07-07
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _POSIX_C_SOURCE 199309

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

#define NOTIFY_SIG  SIGUSR1

/**
 * @brief 信号处置函数
 * 
 * @param sig 
*/
// static void handler(int sig)
// {
//     /* 仅仅是为了中断 sigsuspend */
// }

int main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t numRead;
    sigset_t blockMask, emptyMask;
    // struct sigaction sa;
    siginfo_t info;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        fprintf(stderr, "mq_open\n");
        exit(EXIT_FAILURE);
    }

    if (mq_getattr(mqd, &attr) == -1) {
        fprintf(stderr, "mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        fprintf(stderr, "malloc\n");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sa.sa_handler = handler;
    // if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) {
    //     perror("sigaction");
    //     exit(EXIT_FAILURE);
    // }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }

    // sigemptyset(&emptyMask);

    for (;;) {
        // sigsuspend(&emptyMask);
        if (sigwaitinfo(&blockMask, &info) == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }

        printf("Hello\n");

        printf("%ld %ld\n", (long) info.si_signo, (long) info.si_value.sival_int);

        // if (mq_notify(mqd, &sev) == -1) {
        //     perror("mq_notify");
        //     exit(EXIT_FAILURE);
        // }

        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0)
            printf("Read %ld bytes\n", (long) numRead);
        
        if (errno != EAGAIN) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_FAILURE);
}