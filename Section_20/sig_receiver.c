/**
 * @file sig_receiver.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 捕获信号并对其计数
 * @version 0.1
 * @date 2019-04-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static int sigCnt[NSIG];
static volatile sig_atomic_t gotSigint = 0;

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
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0) 
        fprintf(of, "%s<empty signal set>", prefix);
}

/**
 * @brief 信号处置函数
 * 
 * @param sig 信号值
 */
static void handler(int sig)
{
    if (sig == SIGINT)
        gotSigint = 1;
    else 
        sigCnt[sig]++;
}

int main(int argc, char *argv[])
{
    int n, numSecs;
    sigset_t pendingMask, blockingMask, emptyMask;
    struct sigaction sigact;

    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    /* 为所有信号挂载同一个信号处置函数 */
    sigact.sa_handler = handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    for (n = 1; n < NSIG; n++)
        (void) signal(n, handler); /* 忽略错误 */
    
    /* 如果定义了睡眠时间, 则阻塞所有信号并休眠(此时另一个进程发送型号给本进程), 唤醒后打印
       处于等待状态的信号和没有阻塞的信号 */

    if (argc > 1) {
        numSecs = (int)strtol(argv[1], NULL, 10);

        sigfillset(&blockingMask);
        if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1) {
            fprintf(stderr, "sigprocmask");
            exit(EXIT_FAILURE);
        }

        printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
        sleep(numSecs);

        if (sigpending(&pendingMask) == -1) {
            fprintf(stderr, "sinpending");
            exit(EXIT_FAILURE);
        }

        printf("%s: pending signals are: \n", argv[0]);
        printSigset(stdout, "\t\t", &pendingMask);

        sigemptyset(&emptyMask);
        if (sigprocmask(SIG_SETMASK, &emptyMask, NULL)) {
            fprintf(stderr, "sigprocmask\n");
            exit(EXIT_FAILURE);
        }
    }

    /* 不断循环直到收到SIGINT信号 */
    while (!gotSigint)
        continue;
    
    for (n = 1; n < NSIG; n++) {
        if (sigCnt[n] != 0) {
            printf("%s: signal %d caught %d time%s\n", argv[0], n, sigCnt[n],
                        (sigCnt[n] == 1) ? "" : "s");
        }
    }

    return 0;
}