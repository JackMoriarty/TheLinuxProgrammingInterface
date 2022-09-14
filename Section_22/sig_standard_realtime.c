/**
 * @file sig_standard_realtime.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 测试 Linux 对标准信号和实时信号的传递顺序处理
 * @version 0.1
 * @date 2019-04-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * @brief 标准信号处理函数
 * 
 * @param sig 信号值
 */
void standard_sig_handler(int sig)
{
    printf("standard signal\n");
}

/**
 * @brief 实时信号处理函数
 * 
 * @param sig 信号值
 */
void realtime_sig_handler(int sig)
{
    printf("realtime signal\n");
}

int main(int argc, char *argv[])
{
    struct sigaction standard_sig_act;
    struct sigaction realtime_sig_act;
    sigset_t sig_mask;

    printf("pid is %d\n", getpid());

    /* 设置相关变量 */
    standard_sig_act.sa_handler = standard_sig_handler;
    if (sigemptyset(&(standard_sig_act.sa_mask)) == -1) {
        fprintf(stderr, "sigemptyset standard_sig_act.sa_mask\n");
        exit(EXIT_FAILURE);
    }
    standard_sig_act.sa_flags = 0;
    realtime_sig_act.sa_handler = realtime_sig_handler;
    if (sigemptyset(&(realtime_sig_act.sa_mask)) == -1) {
        fprintf(stderr, "sigemptyset realtime_sig_act.sa_mask\n");
        exit(EXIT_FAILURE);
    }
    realtime_sig_act.sa_flags = 0;

    /* 挂载信号处理函数 */
    if (sigaction(SIGUSR1, &standard_sig_act, NULL)) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGRTMIN, &realtime_sig_act, NULL)) {
        perror("sigaction SIGRTMIN");
        exit(EXIT_FAILURE);
    }

    /* 阻塞这两个信号 */
    if (sigemptyset(&sig_mask) == -1) {
        fprintf(stderr, "sigempty block_mask\n");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&sig_mask, SIGUSR1) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&sig_mask, SIGRTMIN) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        perror("sigprocmask set block\n");
        exit(EXIT_FAILURE);
    }

    printf("waitting for 15 s\n");
    sleep(15);

    /* 取消阻塞 */
    if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
        perror("sigprocmask rm block\n");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}