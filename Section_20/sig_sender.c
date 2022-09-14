/**
 * @file sig_sender.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 发送多个信号
 * @version 0.1
 * @date 2019-04-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int numSigs, sig, j;
    pid_t pid;
     if (argc < 4 || strcmp(argv[1], "--help") == 0) {
         fprintf(stderr, "%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
         exit(EXIT_FAILURE);
     }

    pid = strtol(argv[1], NULL, 10);
    numSigs = strtol(argv[2], NULL, 10);
    sig = strtol(argv[3], NULL, 10);

    /* 向接收者发送信号 */
    printf("%s: sending signal %d to process %ld %d times\n", 
                    argv[0], sig, (long)pid, numSigs);

    for (j = 0; j < numSigs; j++) {
        if (kill(pid, sig) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }

    /* 第四个参数定义时, 发送那个信号 */
    if (argc > 4) {
        sig = strtol(argv[4], NULL, 10);
        if (kill(pid, sig) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }

    printf("%s: exiting\n", argv[0]);
    return 0;
}