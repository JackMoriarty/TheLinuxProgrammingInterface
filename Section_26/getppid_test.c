/**
 * @file getppid_test.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 验证子进程的父进程终止时, getppid 返回 1
 * @version 0.1
 * @date 2019-04-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        /* sleep for 1s */
        sleep(1);
        printf("ppid: %ld\n", (long)getppid());
        _exit(EXIT_SUCCESS);

    default:
        exit(EXIT_SUCCESS);
    }
}