/**
 * @file timer_create_test.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 验证timer_create函数当evp设为NULL时的默认配置
 * @version 0.1
 * @date 2019-04-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t got_alarm = 0;

static void sigalrm_handler(int sig, siginfo_t *siginfo, void *ucontext)
{
    /* 输出定时器ID */
    printf("From handler:\t%d\n", siginfo->si_value.sival_int);
    got_alarm = 1;
}

int main(int argc, char *argv[])
{
    timer_t timer_id;
    struct itimerspec new_value;
    struct sigaction sigact;

    /* 挂载信号处理函数 */
    sigact.sa_sigaction = sigalrm_handler;
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGALRM, &sigact, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* 创建定时器 */
    if (timer_create(CLOCK_REALTIME, NULL, &timer_id) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    printf("From main:\t%d\n", timer_id);

    /* 设置定时器 */
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;
    new_value.it_value.tv_sec = 5;
    new_value.it_value.tv_nsec = 0;

    if (timer_settime(timer_id, 0, &new_value, NULL) == -1) {
        perror("timer_settime");
        goto exit;
    }

    /* 等待定时器到期 */
    while(!got_alarm);

exit:
    if (timer_delete(timer_id) == -1) {
        perror("timer_delete");
        exit(EXIT_FAILURE);
    }

    return 0;
}