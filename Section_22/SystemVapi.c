/**
 * @file SystemVapi.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 利用POSIX函数实现System V的函数
 * @version 0.1
 * @date 2019-04-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#ifdef TEST
void sig_handler(int sig)
{
    printf("Hello\n");
}
#endif

/**
 * @brief 设置信号
 * 
 * @param sig 信号值
 * @param handler 信号处理函数
 */
void (*my_sigset(int sig, void (*handler)(int)))(int)
{
    struct sigaction sigact;
    sigset_t block_set;
    void (*old_handler)(int);

    // 获取之前的信号处理
    if (sigaction(sig, NULL, &sigact)) {
        perror("sigaction get\n");
        return (void (*)(int))-1;
    }

    old_handler = sigact.sa_handler;
    
    // 获取当前进程阻塞信号掩码
    if (sigprocmask(SIG_BLOCK, NULL, &block_set) == -1) {
        perror("sigprocmask get block\n");
        return (void (*)(int))-1;
    }
    
    if (handler != SIG_HOLD) {
        sigact.sa_handler = handler;

        if (sigaction(sig, &sigact, NULL)) {
            perror("sigaction\n");
            return (void (*)(int))-1;
        }
        if (sigdelset(&block_set, sig) == -1) {
            perror("sigdelset");
            return (void (*)(int))-1;
        }
        if (sigprocmask(SIG_SETMASK, &block_set, NULL) == -1) {
            perror("sigprocmask set block\n");
            return (void (*)(int))-1;
        }

        return old_handler;
    } else {

        // 判断当前信号是否被阻塞
        if (sigismember(&block_set, sig)) {
            return SIG_HOLD;
        } else {
            if (sigaddset(&block_set, sig) == -1) {
                perror("sigaddset");
                return (void (*)(int))-1;
            }
            if (sigprocmask(SIG_SETMASK, &block_set, NULL) == -1) {
                perror("sigprocmask set block\n");
                return (void (*)(int))-1;
            }
            return sigact.sa_handler;
        }
    }
}

/**
 * @brief 将信号添加到进程信号掩码中
 * 
 * @param sig 信号值
 * @return int return 0 on success, or -1 on error
 */
int my_sighold(int sig)
{
    sigset_t sigset;

    /* 获取当前进程信号掩码 */
    if (sigprocmask(SIG_BLOCK, NULL, &sigset) == -1) {
        perror("sigprocmask get sigset");
        return -1;
    }

    /* 将该信号添加到信号集中 */
    if (sigaddset(&sigset, sig) == -1) {
        perror("sigaddset");
        return -1;
    }

    /* 设置当前进程信号掩码 */
    if (sigprocmask(SIG_SETMASK, &sigset, NULL) == -1) {
        perror("sigprocmask set sigset");
        return -1;
    }

    return 0;
}

/**
 * @brief 从信号掩码中移除一个信号
 * 
 * @param sig 信号值
 * @return int 0 on success, or -1 on error
 */
int my_sigrelse(int sig)
{
    sigset_t sigset;

    /* 获取当前进程信号掩码 */
    if (sigprocmask(SIG_BLOCK, NULL, &sigset) == -1) {
        perror("sigprocmask get sigset");
        return -1;
    }

    /* 将该信号添加到信号集中 */
    if (sigdelset(&sigset, sig) == -1) {
        perror("sigdelset");
        return -1;
    }

    /* 设置当前进程信号掩码 */
    if (sigprocmask(SIG_SETMASK, &sigset, NULL) == -1) {
        perror("sigprocmask set sigset");
        return -1;
    }

    return 0;
}

/**
 * @brief 设置指定信号的处理处置为忽略
 * 
 * @param sig 信号
 * @return int 0 on success, or -1 on error
 */
int my_sigignore(int sig)
{
    struct sigaction sigact;

    /* 获取当前信号处置 */
    if (sigaction(sig, NULL, &sigact) == -1) {
        perror("sigaction get sigaction");
        return -1;
    }

    /* 设置当前信号处置为SIG_IGN */
    sigact.sa_handler = SIG_IGN;

    /* 设置当前信号处置 */
    if (sigaction(sig, &sigact, NULL) == -1) {
        perror("sigaction set sigaction");
        return -1;
    }

    return 0;
}

/**
 * @brief 从进程信号掩码中移除一个信号, 随后将停止进程, 直到有信号到达
 * 
 * @param sig 指定信号
 * @return int always return -1 with errno set to EINTR
 */
int my_sigpause(int sig)
{
    sigset_t mask;
    
    sigfillset(&mask);
    if (sigdelset(&mask, sig) == -1) {
        perror("sigdelset");
        return -1;
    }
    return sigsuspend(&mask);
}

#ifdef TEST
int main(int argc, char *argv[])
{
    printf("pid is %d\n", getpid());

    /* 设置信号处理函数 */
    my_sigset(SIGUSR1, sig_handler);
    printf("waiting\n");
    pause();
    printf("waiting\n");
    my_sighold(SIGUSR1);
    sleep(5);
    printf("ok\n");
    printf("waiting\n");
    my_sigrelse(SIGUSR1);
    printf("sleeping for 5s\n");
    sleep(5);
    my_sigignore(SIGUSR1);
    printf("sleeping for 5s\n");
    sleep(5);
    printf("ok\n");
    my_sigset(SIGUSR1, sig_handler);
    printf("set ok\n");
    my_sigpause(SIGUSR1);
    printf("waiting for 5s\n");
    sleep(5);
    return 0;
}
#endif