/**
 * @file sigwaitinfo_alarm.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 使用sigwaitinfo函数接收定时器
 * @version 0.1
 * @date 2019-04-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define TIMER_SIG SIGRTMAX

/**
 * @brief 将 "时间+间隔" 的字符串转换为 itimerspec的值
 * 
 * @param str 字符串
 * @param tsp itimerspec 的值
 */
void itimerspecFromStr(char *str, struct itimerspec *tsp)
{
    char *cptr, *sptr;

    cptr = strchr(str, ':');
    if (cptr != NULL)
        *cptr = '\0';
    
    sptr = strchr(str, '/');
    if (sptr != NULL)
        *sptr = '\0';
    
    tsp->it_value.tv_sec = atoi(str);
    tsp->it_value.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
    if (cptr == NULL) {
        tsp->it_interval.tv_sec = 0;
        tsp->it_interval.tv_nsec = 0;
    } else {
        sptr = strchr(cptr + 1, '/');
        if (sptr != NULL)
            *sptr = '\0';
        tsp->it_interval.tv_sec = atoi(cptr + 1);
        tsp->it_interval.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
    }
}

int main(int argc, char *argv[])
{
    struct itimerspec ts;
    struct sigevent sev;
    timer_t *tidlist;
    sigset_t set;
    siginfo_t info;
    int j;

    if (argc < 2) {
        fprintf(stderr, 
            "Usage: %s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL) {
        fprintf(stderr, "malloc\n");
        exit(EXIT_FAILURE);
    }
    
    sigemptyset(&set);
    if (sigaddset(&set, TIMER_SIG) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }

    /* 阻塞定时器产生的信号 */
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* 创建定时器 */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIG;

    for (j = 0; j < argc - 1; j++) {
        itimerspecFromStr(argv[j + 1], &ts);

        /* 允许获得定时器ID */
        sev.sigev_value.sival_ptr = &tidlist[j];
        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1) {
            perror("timer_create");
            exit(EXIT_FAILURE);
        }
        printf("Timer ID: %ld (%s)\n", (long)tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1) {
            perror("timer_settime");
            exit(EXIT_FAILURE);
        }

    }

    while(1) {
        if (sigwaitinfo(&set, &info) == -1)
            perror("sigwaitinfo");
        else
            printf("Get Timer ID: %ld\n", *(long *)info.si_value.sival_ptr);
    }
    
    return 0;
}