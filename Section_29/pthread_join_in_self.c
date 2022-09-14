/**
 * @file pthread_join_in_self.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 线程自身调用pthread_join连接自己
 * @version 0.1
 * @date 2019-04-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

pthread_t t1;

/**
 * @brief 线程函数
 * 
 * @param arg 
 * @return void* 
 */
static void *threadFunc(void *arg)
{
    int s;
    printf("before\n");
    if (pthread_equal(t1, pthread_self()))
    s = pthread_join(pthread_self(), NULL);
    // int s = pthread_join(t1, NULL);
    if (s != 0) {
        fprintf(stderr, "%s\n", strerror(s));
    }
    printf("after\n");
}

int main(int argc, char *argv[])
{
    int s;

    s = pthread_create(&t1, NULL, threadFunc, NULL);
    if (s != 0) {
        fprintf(stderr, "%s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("Massage from main\n");
    // pthread_join(t1, NULL);
    pause();
    exit(EXIT_SUCCESS);
}