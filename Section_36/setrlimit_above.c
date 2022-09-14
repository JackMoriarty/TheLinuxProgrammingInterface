/**
 * @file setrlimit_above.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 程序消耗的资源超过设置的软限制是会怎样
 * @version 0.1
 * @date 2019-05-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>

/**
 * @brief sigxcpu 信号处置函数
 * 
 * @param sig 信号值
 */
static void sigxcpu_handler(int sig)
{
    /* UNSAFE: printf is not asynchronous signal safe */
    printf("Get it!\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sigact;
    struct rlimit rlim;

    /* 注册信号处理函数 */
    sigact.sa_handler = sigxcpu_handler;
    if (sigemptyset(&sigact.sa_mask) == -1) {
        fprintf(stderr, "sigemptyset failed\n");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = 0;
    if (sigaction(SIGXCPU, &sigact, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* 设置上限值 */
    rlim.rlim_cur = 10;
    rlim.rlim_max = 30;
    if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    while(1);

    exit(EXIT_SUCCESS);
}