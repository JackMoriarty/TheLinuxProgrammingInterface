/**
 * @file child_sigchld.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 某个线程子进程终止时SIGCHILD信号发给哪个进程
 * @version 0.1
 * @date 2019-05-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func1(void *arg)
{
    // int sig;
    // sigset_t sigset;

    // /* 阻塞指定信号 */
    // sigemptyset(&sigset);
    // sigaddset(&sigset, SIGCHLD);
    // pthread_sigmask(SIG_SETMASK, &sigset, NULL);
    // switch(fork()) {
    // case -1:
    //     printf("fork fail\n");
    //     break;
    // case 0:
    //     printf("son process\n");
    //     sleep(5);
    //     printf("son process exit\n");
    //     _exit(EXIT_SUCCESS);
    // default:
    //     /* do nothing */
    //     break;
    // }
    // /* 等待信号到达 */
    // printf("servant1 waiting sig\n");
    // sigwait(&sigset, &sig);
    // printf("servant1 exit\n");
    while(1);
}

void *thread_func2(void *arg)
{
    // int sig;
    // sigset_t sigset;

    // /* 阻塞指定信号 */
    // sigemptyset(&sigset);
    // sigaddset(&sigset, SIGCHLD);
    // pthread_sigmask(SIG_SETMASK, &sigset, NULL);
    // // switch(fork()) {
    // // case -1:
    // //     printf("fork fail\n");
    // //     break;
    // // case 0:
    // //     printf("son process\n");
    // //     sleep(5);
    // //     printf("son process exit\n");
    // //     _exit(EXIT_SUCCESS);
    // // default:
    // //     /* do nothing */
    // //     break;
    // // }
    // /* 等待信号到达 */
    // printf("servant2 waiting sig\n");
    // sigwait(&sigset, &sig);
    // printf("servant2 exit\n");
    while(1);
}

void sig_handler(int sig)
{
    printf("Hello\n");
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int sig;
    // sigset_t sigset;

    /* 设置信号处理函数 */
    signal(SIGCHLD, sig_handler);

    pthread_create(&t1, NULL, thread_func1, NULL);
    pthread_create(&t2, NULL, thread_func2, NULL);
    switch(fork()) {
    case -1:
        printf("fork fail\n");
        break;
    case 0:
        printf("son process\n");
        sleep(5);
        printf("son process exit\n");
        _exit(EXIT_SUCCESS);
    default:
        /* do nothing */
        break;
    }
    // /* 阻塞指定信号 */
    // sigemptyset(&sigset);
    // sigaddset(&sigset, SIGCHLD);
    // pthread_sigmask(SIG_SETMASK, &sigset, NULL);

    // /* 等待信号到达 */
    // printf("Master wait sig\n");
    // sigwait(&sigset, &sig);
    // printf("Master get sig\n");

    /* 连接线程 */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /* 退出 */
    exit(EXIT_SUCCESS);
}