/**
 * @file fifo.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief FIFO 调度
 * @version 0.1
 * @date 2019-05-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>

enum boolean {
    FALSE,
    TRUE
};

/**
 * @brief 延时函数, 演示 1/4 秒
 * 
 */
static void delay() {
    struct timeval start;
    struct timeval now;
    double spend_time;

    gettimeofday(&start, NULL);
    while (TRUE) {
        gettimeofday(&now, NULL);
        
        spend_time = 
            now.tv_sec - start.tv_sec + (now.tv_usec - start.tv_usec) / 1000000;
        if (spend_time >= 0.25)
            break;
    }
}

static void fifo_func() {
    int i;
    
    for (i = 0; i < 12; i++) {
        delay();
        printf("Process: %ld; Time: %lf(s)\n", (long) getpid(), (i + 1) * 0.25);

        // if (i % 4 == 3) {
        //     printf("Process: %ld\n", (long) getpid());
        //     sched_yield();
        // }
        if (i % 4 == 3)
            sched_yield();
    }

}

int main(int argc, char *argv[])
{
    int priority;
    struct sched_param param;
    int func_status;
    cpu_set_t set;
    struct rlimit rlim;

    /* 限制在同一个CPU上运行 */
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    func_status = sched_setaffinity(0, sizeof(cpu_set_t), &set);
    if (func_status == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    /* 设定FIFO调度策略 */
    priority = sched_get_priority_max(SCHED_FIFO);
    if (priority == -1) {
        perror("sched_get_priority_max");
        exit(EXIT_FAILURE);
    }
    param.sched_priority = priority;
    func_status = sched_setscheduler(0, SCHED_FIFO, &param);
    if (func_status == -1) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    /* 防止实时进程锁住系统 */
    rlim.rlim_cur = rlim.rlim_max = 50;
    if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        fifo_func();
        break;
    default:
        fifo_func();
        break;
    }

    exit(EXIT_SUCCESS);
}