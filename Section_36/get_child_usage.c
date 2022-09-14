/**
 * @file get_child_usage.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 使用 getrusage 获取wait调用等待的子进程的相关信息
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
#include <sys/time.h>
#include <sys/wait.h>

enum boolean {
    FALSE,
    TRUE
};

/**
 * @brief 显示进程资源的使用
 * 
 * @param msg 前缀信息
 * @param who 查询资源使用信息的进程类别
 * @return int 0 表示成功, -1 表示失败
 */
static int print_rusage(const char *msg, int who)
{
    int retval;
    struct rusage res_usage;
    double utime;
    double stime;

    retval = getrusage(who, &res_usage);
    if (retval == -1)
        return -1;
    
    printf("%s usage:\n", msg);
    utime = res_usage.ru_utime.tv_sec + res_usage.ru_utime.tv_usec / 1000000;
    stime = res_usage.ru_stime.tv_sec + res_usage.ru_stime.tv_usec / 1000000;
    printf("User CPU time: %lf\n", utime);
    printf("System CPU time: %lf\n", stime);

    return 0;
}

/**
 * @brief 延时函数, 演示 4 秒
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
        if (spend_time >= 4)
            break;
    }
}

int main(int argc, char *argv[])
{
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        delay();
        exit(EXIT_SUCCESS);
    default:
        print_rusage("before wait", RUSAGE_CHILDREN);
        wait(NULL);
        print_rusage("after wait", RUSAGE_CHILDREN);
    }
    exit(EXIT_SUCCESS);
}