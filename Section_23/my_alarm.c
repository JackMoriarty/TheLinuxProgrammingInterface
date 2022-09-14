/**
 * @file my_alarm.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 利用setitimer实现alarm
 * @version 0.1
 * @date 2019-04-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

/**
 * @brief 利用settimer实现的alarm
 * 
 * @param seconds 定时器到期秒数
 * @return unsigned int 
 */
unsigned int my_alarm(unsigned int seconds)
{
    struct itimerval new_value;
    struct itimerval old_value;

    /* 获取之前定时器剩余时间 */
    getitimer(ITIMER_REAL, &old_value);

    /* 设置定时器 */
    new_value.it_value.tv_sec = seconds;
    new_value.it_value.tv_usec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &new_value, NULL);

    return old_value.it_value.tv_sec;
}

#ifdef TEST

static volatile sig_atomic_t got_alarm = 0;

void sigalrm_handler(int sig)
{
    got_alarm = 1;
    printf("GET\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sigact;

    sigact.sa_handler = sigalrm_handler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);

    if (sigaction(SIGALRM, &sigact, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    my_alarm(5);

    while(!got_alarm);

}
#endif