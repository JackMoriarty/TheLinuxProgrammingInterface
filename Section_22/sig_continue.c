/**
 * @file sig_continue.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * @brief SIGCONT信号的处理器函数
 * 
 * @param sig 信号值
 */
void sigcont_handler(int sig)
{
    printf("I am in handler func\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sigact;
    sigset_t block_mask;

    printf("pid is %d\n", getpid());
    /* 挂载SIGCONT信号的处理函数 */
    sigact.sa_handler = sigcont_handler;
    if(sigemptyset(&sigact.sa_mask) == -1) {
        fprintf(stderr, "sigemptyset\n");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = 0;
    if (sigaction(SIGCONT, &sigact, NULL)) {
        perror("sigaction\n");
        exit(EXIT_FAILURE);
    }

    /* 阻塞SIGCONT信号 */
    if (sigemptyset(&block_mask) == -1) {
        fprintf(stderr, "sigempty block_mask\n");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&block_mask, SIGCONT) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }
    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask set block\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Now we wait signal <Ctrl + Z>, sleep 20s\n");
    sleep(20);

    printf("We recive SIGCONT signal\n");
    printf("But SIGCONT sighandler do not run, we will rm block\n");
    sleep(5);

    /* 解除对SIGCONT信号的阻塞 */
    if (sigprocmask(SIG_UNBLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask rm block\n");
        exit(EXIT_FAILURE);
    }
    printf("rm the block success\n");

    return 0;
}