/**
 * @file svshm_ls.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 显示系统中所有的sysv共享内存
 * @version 0.1
 * @date 2019-07-04
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int maxindex, i;
    struct shminfo info;
    struct shmid_ds buf;
    char shm_atime[30];
    char shm_dtime[30];
    char shm_ctime[30];

    /* 获取sysv共享内存 */
    maxindex = shmctl(0, IPC_INFO, (struct shmid_ds *)&info);
    if (maxindex == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    /* 遍历并输出 */
    printf("max index: %d\n", maxindex);
    printf("seg_sz\t\tatime\t\t\t\tdtime\t\t\t\tctime\t\t\t\tcpid\t\tlpid\t\tnattch\n");
    for (i = 0; i < maxindex; i++) {
        if (shmctl(i, SHM_STAT, &buf) == -1) {
            if (errno != EINVAL) {
                perror("shmctl-SHM_STAT");
                exit(EXIT_FAILURE);
            } else {
                continue;
            }
        }

        /* 输出 */
        // 不检查错误, 因为懒
        ctime_r(&(buf.shm_atime), shm_atime);
        shm_atime[strlen(shm_atime) - 1] = '\0';
        ctime_r(&(buf.shm_dtime), shm_dtime);
        shm_dtime[strlen(shm_dtime) - 1] = '\0';
        ctime_r(&(buf.shm_ctime), shm_ctime);
        shm_ctime[strlen(shm_ctime) - 1] = '\0';

        printf("%-10ld\t%s\t%s\t%s\t%-10ld\t%-8ld\t%-10ld\n", 
                (long)buf.shm_segsz, shm_atime, shm_dtime, shm_ctime, 
                (long)buf.shm_cpid, (long)buf.shm_lpid, (long)buf.shm_nattch);

    }

    exit(EXIT_FAILURE);
}