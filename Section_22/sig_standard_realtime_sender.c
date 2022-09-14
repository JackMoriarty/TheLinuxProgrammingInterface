/**
 * @file sig_standard_realtime_sender.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 向 sig_standard_realtime 发送信号
 * @version 0.1
 * @date 2019-04-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid;
    union sigval sv;
    scanf("%d", &pid);

    /* 发送信号 */
    kill(pid, SIGUSR1);
    sigqueue(pid, SIGRTMIN, sv);
    sigqueue(pid, SIGRTMIN, sv);
    kill(pid, SIGUSR1);

    return 0;
}