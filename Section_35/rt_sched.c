/**
 * @file rt_sched.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 设置进程的调度方式及优先级
 * @version 0.1
 * @date 2019-05-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char *argv[])
{
    int policy;
    char policy_chr;
    int priority;
    int s;
    struct sched_param param;
    uid_t uid;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s policy priority command [arg...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    policy_chr = argv[1][0];
    policy = policy_chr == 'r' ? SCHED_RR : 
            (policy_chr == 'f' ? SCHED_FIFO : SCHED_OTHER);
    
    priority = atoi(argv[2]);
    printf("priority: %ld\n", priority);

    /* 修改调度策略和优先级 */
    param.sched_priority = priority;
    s = sched_setscheduler(0, policy, &param);
    if (s == -1) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    /* 丢弃特权ID */
    uid = getuid();
    if (setuid(uid) == -1) {
        perror("setuid");
        exit(EXIT_FAILURE);
    }

    /* 启动相应程序 */
    execv(argv[3], &argv[4]);

    /* 应该不会执行到此处 */
    perror("execv");
    exit(EXIT_FAILURE);
}