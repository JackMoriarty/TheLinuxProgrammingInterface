/**
 * @file svshm_xfr_write.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief system V 共享内存写者程序
 * @version 0.1
 * @date 2019-07-04
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "semun.h"
#include "svshm_xfr.h"

int main(int argc, char *argv[])
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun arg, dummy;
    struct sembuf semops;

    /* 创建信号量并完成相应初始化 */
    semid = semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | OBJ_PERMS);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    arg.val = 1;
    if (semctl(semid, WRITE_SEM, SETVAL, arg) == -1) {
        perror("Init write sem");
        exit(EXIT_FAILURE);
    }
    arg.val = 0;
    if (semctl(semid, READ_SEM, SETVAL, arg) == -1) {
        perror("Init read sem");
        exit(EXIT_FAILURE);
    }

    /* 创建共享内存并连接 */
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | IPC_EXCL | OBJ_PERMS);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    /* 进行数据传递 */
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {

        /* 预留write信号量 */
        semops.sem_num = WRITE_SEM;
        semops.sem_op = -1;
        semops.sem_flg = 0;
        if (semop(semid, &semops, 1) == -1) {
            perror("semop write sem -1");
            exit(EXIT_FAILURE);
        }

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read from stdin");
            exit(EXIT_FAILURE);
        }

        /* 释放read信号量 */
        semops.sem_num = READ_SEM;
        semops.sem_op = 1;
        semops.sem_flg = 0;
        if (semop(semid, &semops, 1) == -1) {
            perror("semop read sem +1");
            exit(EXIT_FAILURE);
        }

        /* 判断是否到达末尾 */
        if (shmp->cnt == 0)
            break;
    }

    /* 等待读者程序完成数据读取 */
    semops.sem_num = WRITE_SEM;
    semops.sem_op = -1;
    semops.sem_flg = 0;
    if (semop(semid, &semops, 1) == -1) {
        perror("semop write sem -1 before rm shm");
        exit(EXIT_FAILURE);
    }

    /* 移除信号量和共享内存 */
    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
        perror("semctl-IPC_RMID");
        exit(EXIT_FAILURE);
    }
    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl-IPC_RMID");
        exit(EXIT_FAILURE);
    }

    printf("Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}