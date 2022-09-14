/**
 * @file svsem_ls.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 显示系统中所有sysv 信号量集
 * @version 0.1
 * @date 2019-07-03
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "semun.h"

int main(int argc, char *argv[])
{
    int maxind, ind, i;
    struct semid_ds ds;
    struct seminfo seminfo;
    union semun arg, dummy;
    char otime_buf[100];
    char ctime_buf[100];

    /* 获取当前系统中信号量的最大值 */
    maxind = semctl(0, 0, SEM_INFO, (struct semid_ds *)&seminfo);
    if (maxind == -1) {
        fprintf(stderr, "semctl-SEM_INFO: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("maxind: %d\n", maxind);
    printf("index        sem_otime                     sem_ctime \
                    sem_nsems\n");

    /* 获取信息并展示 */
    for (ind = 0; ind <= maxind; ind++) {
        /* 获取信号量集合信息 */
        arg.buf = &ds;
        if (semctl(ind, 0, SEM_STAT, arg) == -1) {
            if (errno != EINVAL) {
                fprintf(stderr, "semctl-IPC_STAT: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            } else {
                perror("semctl-IPC_STAT");
                continue;
            }

        }
        ctime_r(&(ds.sem_ctime), ctime_buf);
        ctime_buf[strlen(ctime_buf) - 1] = '\0';
        ctime_r(&(ds.sem_otime), otime_buf);
        otime_buf[strlen(otime_buf) - 1] = '\0';
        
        printf("%-5d        %20s      %20s      %-5ld\n", 
                ind, otime_buf, ctime_buf, (long)ds.sem_nsems);
    }

    exit(EXIT_SUCCESS);
}