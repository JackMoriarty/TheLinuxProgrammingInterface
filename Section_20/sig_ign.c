/**
 * @file sig_ign.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 将等待信号的处置改为SIG_IGN时, 程序不会看到(捕获)信号
 * @version 0.1
 * @date 2019-04-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    struct sigaction sigact;

    // 挂载SIGINT信号处置函数
    sigact.sa_handler = SIG_IGN;
    if (sigemptyset(&(sigact.sa_mask)) == -1) {
        fprintf(stderr, "sigemptyset\n");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = 0;
    if (sigaction(SIGINT, &sigact, NULL) == -1) {
        char err_msg[50];
        snprintf(err_msg, 50, "sigaction SIGINT");
        perror(err_msg);
        exit(EXIT_FAILURE);
    }

    // 循环等待
    while(1);
    return 0;
}