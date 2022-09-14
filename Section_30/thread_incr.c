/**
 * @file thread_incr.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 两线程以错误的方式递增全局变量的值
 * @version 0.1
 * @date 2019-04-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct thread_arg {
        pthread_t t;
        int loop;
};

static int glob = 0;

/**
 * @brief 线程函数
 * 
 * @param arg 循环次数
 * @return void* 永远返回 NULL
 */
static void *thread_func(void *arg)
{
    struct thread_arg t = *((struct thread_arg *) arg);
    int loc, j;

    for (j = 0; j < t.loop; j++) {
        loc = glob;
        loc++;
        glob = loc;
        printf("Thread ID = %ld(%ld), glob = %d\n", pthread_self(), (long) t.t, glob);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    // pthread_t t1, t2;
    int loops, s;
    struct thread_arg t1, t2;

    loops = (argc > 1) ? atoi(argv[1]) : 10000000;
    t1.loop = loops;
    t2.loop = loops;

    s = pthread_create(&t1.t, NULL, thread_func, &t1);
    if (s!= 0) {
        fprintf(stderr, "create t1:%s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t2.t, NULL, thread_func, &t2);
    if (s != 0) {
        fprintf(stderr, "create t2:%s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_join(t1.t, NULL);
    if (s != 0) {
        fprintf(stderr, "join t1:%s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t2.t, NULL);
    if (s != 0) {
        fprintf(stderr, "join t2:%s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}