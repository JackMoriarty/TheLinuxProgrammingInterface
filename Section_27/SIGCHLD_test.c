/**
 * @file SIGCHLD_test.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 27-6 验证程序
 * @version 0.1
 * @date 2019-04-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigchld_handler(int sig)
{
    printf("Hello\n");
}

int main(int argc, char *argv[])
{
    sigset_t block_set, origin_set;
    struct sigaction sigact;

    sigact.sa_handler = sigchld_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;

    if (sigaction(SIGCHLD, &sigact, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&block_set);
    if (sigaddset(&block_set, SIGCHLD) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_SETMASK, &block_set, &origin_set) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
    case -1: 
        perror("fork");
        break;
    
    case 0:
        printf("exiting\n");
        _exit(EXIT_SUCCESS);

    default:
        sleep(2);
        printf("sleep over\n");
        wait(NULL);
        printf("wait ok\n");
        if (sigprocmask(SIG_SETMASK, &origin_set, NULL) == -1) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }

    }


    exit(EXIT_SUCCESS);
}