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
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "semun.h"
#include "svshm_xfr.h"

int main(int argc, char *argv[])
{
    int semid, shm_fd, bytes, xfrs;
    union semun arg, dummy;
    struct shmseg *shmp;
    struct sembuf semops;
    void *mmap_addr;
    

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
    shm_fd = shm_open(SEM_NAME, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS);
    if (shm_fd == -1) {
        perror("writer shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(struct shmseg)) == -1) {
        perror("writer ftruncate");
        exit(EXIT_FAILURE);
    }
    mmap_addr = mmap(NULL, sizeof(struct shmseg), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mmap_addr == MAP_FAILED) {
        perror("write mmap");
        exit(EXIT_FAILURE);
    }
    shmp = mmap_addr;

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
    if (munmap(mmap_addr, sizeof(struct shmseg)) == -1) {
        perror("write munmap");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SEM_NAME) == -1) {
        perror("write shm_unlink");
        exit(EXIT_FAILURE);
    }

    printf("Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}