/**
 * @file my_siginterrupt.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 利用sigaction实现siginterrupt
 * @version 0.1
 * @date 2019-04-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#ifdef TEST
void sig_quit_handler(int sig)
{
    printf("Hello!\n");
}
#endif

/**
 * @brief 自己实现的siginterrupt
 * 
 * @param sig 信号值
 * @param flag 标志位
 * @return int 0 表示成功, -1 表示失败
 */
int my_siginterrupt(int sig, int flag)
{
    struct sigaction sigact;

#ifndef TEST
    if (flag == 1) {
#endif
        /* 获取之前的信号处理 */
        if (sigaction(sig, NULL, &sigact) == -1) {
            perror("sigaction get sig\n");
            return -1;
        }
    #ifdef TEST
        sigact.sa_handler = sig_quit_handler;
        if (flag == 1)
    #endif
        sigact.sa_flags |= SA_RESTART;
        /* 设置SIGQUIT的信号处理 */
        if (sigaction(sig , &sigact, NULL) == -1) {
            perror("sigaction set SIGQUIT\n");
            return -1;
        }
#ifndef TEST
    }
#endif

    return 0;
}

int main(int argc, char *argv[])
{
    char buf[1024];
    int read_num;

    my_siginterrupt(SIGQUIT, 1);

    if ((read_num = read(STDIN_FILENO, buf, 1024)) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    buf[read_num] = '\0';
    printf("%s", buf);
    return 0;
}