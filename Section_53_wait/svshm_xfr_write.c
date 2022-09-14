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
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include "svshm_xfr.h"

int main(int argc, char *argv[])
{
    int shmid, bytes, xfrs;
    struct shmseg *shmp;
    sem_t *sem_read, *sem_write;

    /* 创建信号量并完成相应初始化 */
    sem_read = sem_open(SEM_READ, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS, 0);
    if (sem_read == SEM_FAILED) {
        perror("sem_open-sem_read");
        exit(EXIT_FAILURE);
    }
    sem_write = sem_open(SEM_WRITE, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS, 1);
    if (sem_write == SEM_FAILED) {
        perror("sem_open-sem_write");
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
        if (sem_wait(sem_write) == -1) {
            perror("sem_wait-sem_write");
            exit(EXIT_FAILURE);
        }

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            perror("read from stdin");
            exit(EXIT_FAILURE);
        }

        /* 释放read信号量 */
        if (sem_post(sem_read) == -1) {
            perror("sem_post-sem_read");
            exit(EXIT_FAILURE);
        }

        /* 判断是否到达末尾 */
        if (shmp->cnt == 0)
            break;
    }

    /* 等待读者程序完成数据读取 */
    if (sem_wait(sem_write) == -1) {
        perror("sem_wait-sem_write");
        exit(EXIT_FAILURE);
    }

    /* 移除信号量和共享内存 */
    if (sem_unlink(SEM_WRITE) == -1)
        perror("sem_unlink-write");
    if (sem_unlink(SEM_READ) == -1)
        perror("sem_unlink-read");
    if (sem_close(sem_read) == -1) {
        perror("sem_close-sem_read");
        exit(EXIT_FAILURE);
    }
    if (sem_close(sem_write) == -1) {
        perror("sem_close-sem_write");
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