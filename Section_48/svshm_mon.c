/**
 * @file svshm_mon.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-07-04
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int shm_id;
    struct shmid_ds buf;
    char shm_atime[30];
    char shm_dtime[30];
    char shm_ctime[30];

    /* 参数检查 */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s shmid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 获取共享内存ID并获取shmid_ds结构 */
    shm_id = atoi(argv[1]);
    if (shmctl(shm_id, IPC_STAT, &buf) == -1) {
        perror("shmctl-IPC_STAT");
        exit(EXIT_FAILURE);
    }

    /* 处理结构中的相关信息并进行显示 */
    printf("seg_sz\t\tatime\t\t\t\tdtime\t\t\t\tctime\t\t\t\tcpid\t\tlpid\t\tnattch\n");
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
    
    exit(EXIT_FAILURE);
}