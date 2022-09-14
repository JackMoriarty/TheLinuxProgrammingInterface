/**
 * @file one_time_init.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现pathread_once
 * @version 0.1
 * @date 2019-04-29
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct once_t {
    pthread_mutex_t lock;
    int status;
} once_t;

once_t cont = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .status = 0
};

/**
 * @brief 一次调用
 * 
 * @param control 静态变量, 存储某函数是否调用
 * @param init 待调用的函数
 * @return int 0 表示成功, 或返回正的错误码
 */
int one_time_init(once_t *control, void (*init)(void))
{
    int s;
    int status;

    // 互斥访问control
    /* 临界区开始 */
    s = pthread_mutex_lock(&(control->lock));
    if (s != 0)
        return s;
    status = control->status;
    if (control->status == 0)
        control->status = 1;
    s = pthread_mutex_unlock(&(control->lock));
    if (s != 0)
        return s;
    /* 临界区结束 */

    if (status == 0) {
        init();
    }

    return 0;
}

void hello()
{
    printf("Hello\n");
}

void * thread_func(void *arg)
{
    one_time_init(&cont, hello);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    int s;

    one_time_init(&cont, hello);

    s = pthread_create(&t1, NULL, thread_func, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t1, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}