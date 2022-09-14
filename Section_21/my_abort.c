/**
 * @file my_abort.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 自己实现abort
 * @version 0.1
 * @date 2019-04-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef TEST
void sigabrt_handler(int sig)
{
    printf("Hello!\n");
}
#endif

/**
 * @brief 自己实现的abort函数
 * 
 */
void my_abort()
{
    struct sigaction sigact;
    /* 向自己发送 SIGABRT信号 */
    if (raise(SIGABRT)) {
        perror("raise");
        exit(EXIT_FAILURE);
    }

    /* 此时进程应当已经终止, 如果没有, 恢复默认信号处置函数, 重新发送信号 */
#ifdef TEST
    printf("Now we have to retry\n");
#endif
    if (sigaction(SIGABRT, NULL, &sigact)) {
        perror("get sigaction");
        exit(EXIT_FAILURE);
    }
    sigact.sa_handler = SIG_DFL;
    if (sigaction(SIGABRT, &sigact, NULL)) {
        perror("set sigaction");
        exit(EXIT_FAILURE);
    }
    if (raise(SIGABRT)) {
        perror("raise");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[])
{
#ifdef TEST
    /* 设置自己的处置函数 */
    struct sigaction sigact;

    sigact.sa_handler = sigabrt_handler;
    if (sigemptyset(&sigact.sa_mask)) {
        fprintf(stderr, "sigemptyset");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = 0;

    if (sigaction(SIGABRT, &sigact, NULL) == -1) {
        perror("set myself sighandler");
        exit(EXIT_FAILURE);
    }
#endif

    my_abort();
    return 0;
}