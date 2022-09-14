/**
 * @file psem_timedwait.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用 sem_timedwait 来递减一个 POSIX 信号量
 * @version 0.1
 * @date 2019-07-08
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _POSIX_C_SOURCE     199309L
#define _XOPEN_SOURCE       600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    sem_t *sem;
    long overtime = 10;
    struct timespec abs_timeout;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s sem-name [overtime]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 3)
        overtime = atol(argv[2]);

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_REALTIME, &abs_timeout) == -1) {
        perror("clock_gettime-CLOCK_REALTIME");
        exit(EXIT_SUCCESS);
    }
    abs_timeout.tv_sec += overtime;

    if (sem_timedwait(sem, &abs_timeout) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    printf("%ld sem_wait() succeeded\n", (long) getpid());

    exit(EXIT_FAILURE);
}