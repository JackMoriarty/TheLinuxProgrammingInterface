/**
 * @file orphan_sig.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 孤儿进程组的进程成员收到SIGTTIN, SIGTTOU, SIGTSTP 的处理
 * @version 0.1
 * @date 2019-05-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sig_usr1_handler(int sig)
{
    printf("sig_usr1 recieve\n");
}

int main(int argc, char *argv[])
{
    int child_id;
    struct sigaction sigact;

    /* 为 SIGUSR1 信号设置信号处置 */
    sigact.sa_handler = sig_usr1_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;

    if (sigaction(SIGUSR1, &sigact, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // if (sigaction(SIGTSTP, &sigact, NULL) == -1) {
    //     perror("sigaction");
    //     exit(EXIT_FAILURE);
    // }
    
    switch (child_id = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        /* sleep to wait parent process exit */
        sleep(5);
        printf("child process wake up\n");
        while (1) {
            pause();
            printf("recieve a signal\n");
        }
        /* should not arrive there */
        break;

    default:
        printf("child process id is %ld\n", (long) child_id);
        break;
    }

    exit(EXIT_SUCCESS);
}