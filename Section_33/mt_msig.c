/**
 * @file mt_msig.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 同一进程的不同线程可以拥有不同的pending信号
 * @version 0.1
 * @date 2019-05-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t sig_sended = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sig_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief 打印信号集
 * 
 * @param of 输出流
 * @param prefix 前缀
 * @param sigset 信号集
 */
void printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s %d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0) 
        fprintf(of, "%s <empty signal set>", prefix);
}

void *thread_func(void *arg)
{
    pthread_t my_id;
    int s;
    sigset_t spend_sigset;
    char msg[20];

    /* 获得线程ID */
    my_id = pthread_self();
    printf("Thread %ld wait signal\n", (long) my_id);
    
    /* sleep for 10s */
    /* UNSAFE: sleep is not mt-safe, for mt-safe, can block SIGCHLD */
    sleep(5);

    /* 获取阻塞的信号 */
    s = sigpending(&spend_sigset);
    if (s != 0) {
        /* UNSAFE: strerror is not mt-safe */
        fprintf(stderr, "sigpending: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    snprintf(msg, 20, "%ld", (long) my_id);

    printSigset(stdout, msg, &spend_sigset);
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    sigset_t block_set;
    int s;

    /* 阻塞SIGUSR1 和 SIGUSR2 信号 */
    if (sigemptyset(&block_set) == -1) {
        fprintf(stderr, "Fail to empty blockset\n");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&block_set, SIGUSR1) == -1) {
        perror("Fail to add SIGUSR1:");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&block_set, SIGUSR2) == -1) {
        perror("Fail to add SIGUSR2:");
        exit(EXIT_FAILURE);        
    }
    if (sigprocmask(SIG_SETMASK, &block_set, NULL) == -1) {
        perror("Fail to set mask:");
        exit(EXIT_FAILURE);
    }

    /* 创建两个子线程 */
    s = pthread_create(&t1, NULL, thread_func, NULL);
    if (s != 0) {
        fprintf(stderr, "create t1: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t2, NULL, thread_func, NULL);
    if (s != 0) {
        fprintf(stderr, "create t2: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    /* 发送信号 */
    s = pthread_kill(t1, SIGUSR1);
    if (s != 0) {
        fprintf(stderr, "kill t1: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_kill(t2, SIGUSR2);
    if (s != 0) {
        fprintf(stderr, "kill t2: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    /* 连接子线程 */
    s = pthread_join(t1, NULL);
    if (s != 0) {
        fprintf(stderr, "join t1: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t2, NULL);
    if (s != 0) {
        fprintf(stderr, "join t2: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}   