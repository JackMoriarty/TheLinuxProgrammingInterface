/**
 * @file svshm_xfr_read.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief system V 共享内存读者程序
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
    int semid, shm_fd, shmid, xfrs, bytes;
    struct shmseg *shmp;
    struct sembuf semops;
    void *mmap_addr;


    /* 获取信号量和共享内存 */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    shm_fd = shm_open(SEM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("writer shm_open");
        exit(EXIT_FAILURE);
    }
    mmap_addr = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (mmap_addr == MAP_FAILED) {
        perror("write mmap");
        exit(EXIT_FAILURE);
    }
    shmp = mmap_addr;
    
    /* 接收数据并显示 */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        /* 预留 read 信号量 */
        semops.sem_num = READ_SEM;
        semops.sem_op = -1;
        semops.sem_flg = 0;
        if (semop(semid, &semops, 1) == -1) {
            perror("semop read sem -1");
            exit(EXIT_FAILURE);
        }

        if (shmp->cnt == 0)
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            perror("partial/failed write");

        /* 释放 write 信号量 */
        semops.sem_num = WRITE_SEM;
        semops.sem_op = 1;
        semops.sem_flg = 0;
        if (semop(semid, &semops, 1) == -1) {
            perror("semop write sem 1");
            exit(EXIT_FAILURE);
        }
    }

    /* 断开共享内存连接 */
    if (munmap(mmap_addr, sizeof(struct shmseg)) == -1) {
        perror("write munmap");
        exit(EXIT_FAILURE);
    }

    /* 释放 write 信号量, 使其能够执行清理操作 */
    semops.sem_num = WRITE_SEM;
    semops.sem_op = 1;
    semops.sem_flg = 0;
    if (semop(semid, &semops, 1) == -1) {
        perror("semop write sem 1");
        exit(EXIT_FAILURE);
    }

    printf("Receieved %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}