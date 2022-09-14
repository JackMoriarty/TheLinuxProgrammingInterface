/**
 * @file rusage.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 获取程序的资源使用状况
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
#include <sys/wait.h>

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

int main(int argc, char *argv[])
{
    int child_retval;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [arg...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        execvp(argv[1], &argv[1]);
        /* should not arrive there */
        perror("execv");
        exit(EXIT_FAILURE);
    
    default:
        wait(&child_retval);
        print_rusage(argv[1], RUSAGE_CHILDREN);
    }

    exit(child_retval);
}