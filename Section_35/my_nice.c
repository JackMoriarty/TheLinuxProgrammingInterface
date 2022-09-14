/**
 * @file my_nice.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现nice命令
 * @version 0.1
 * @date 2019-05-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

int main(int argc, char *argv[])
{
    int current_nice;
    int new_nice;
    if (argc == 1) {
        errno = 0;
        current_nice = getpriority(PRIO_PROCESS, 0);
        if (current_nice == -1 && errno != 0) {
            perror("getpriority");
            exit(EXIT_FAILURE);
        }
        printf("%d\n", current_nice);
    } else if (argc > 2) {
        int s;
        new_nice = atoi(argv[1]);
        s = setpriority(PRIO_PROCESS, 0, new_nice);
        if (s == -1) {
            perror("setpriority");
            exit(EXIT_FAILURE);
        }

        execv(argv[2], &argv[2]);

        /* should not arrive there */
        perror("execv");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Usage: %s [NICE COMMAND [ARG]...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
