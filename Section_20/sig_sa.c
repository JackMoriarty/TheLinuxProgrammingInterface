/**
 * @file sig_sa.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 验证SA_RESETHAND和SA_NODEFER标志
 * @version 0.1
 * @date 2019-04-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * @brief SIGQUIT处置函数
 * 
 * @param sig 信号值
 */
void sig_quit_handler(int sig)
{
    printf("quit!\n");
}

/**
 * @brief SIGINFO处置函数
 * 
 * @param sig 信号值
 */
void sig_int_handler(int sig)
{
    while(1) {
        printf("int!\n");
        sleep(5);
    }
        
}

int main(int argc, char *argv[])
{
    struct sigaction sigact;

    // 设置信号处理函数
    sigact.sa_handler = sig_quit_handler;
    if (sigemptyset(&(sigact.sa_mask)) == -1) {
        fprintf(stderr, "sigemptyset\n");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = SA_RESETHAND;
    if (sigaction(SIGQUIT, &sigact, NULL) == -1) {
        perror("sigaction SIGINFO");
        exit(EXIT_FAILURE);
    }
    sigact.sa_handler = sig_int_handler;
    if (sigemptyset(&(sigact.sa_mask)) == -1) {
        fprintf(stderr, "sigemptyset\n");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&(sigact.sa_mask), SIGQUIT) == -1) {
        fprintf(stderr, "sigaddset\n");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = SA_NODEFER;
    if (sigaction(SIGINT, &sigact, NULL) == -1) {
        perror("sigaction SIGINFO");
        exit(EXIT_FAILURE);
    }
    while(1)
        pause();
}