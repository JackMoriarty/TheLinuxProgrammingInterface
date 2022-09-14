/**
 * @file fork_sem_sync.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用 sysv 信号量机制进行同步
 * @version 0.1
 * @date 2019-07-03
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include "semun.h"

int main(int argc, char *argv[])
{
    int semid;
    int child_id;
    union semun arg;
    struct sembuf sem_op;

    /* 创建信号量 */
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL| S_IWUSR | S_IRUSR);
    if (semid == -1) {
        fprintf(stderr, "create sem failed\n");
        exit(EXIT_FAILURE);
    }

    /* 初始化信号量 */
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        fprintf(stderr, "semctl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* 创建子进程并同步 */
    switch(child_id = fork()) {
        case -1:
            fprintf(stderr, "fork: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        
        case 0: /* 子进程 */
            printf("[%ld] Child started - doing some work\n", (long)getpid());
            sleep(2);
            /* 准备发送同步信息 */
            printf("[%ld] Child about to send sem to parent\n", (long)getpid());
            sem_op.sem_num = 0;
            sem_op.sem_op = -1;
            sem_op.sem_flg = 0;
            if (semop(semid, &sem_op, 1) == -1) {
                fprintf(stderr, "Child semop failed: %s\n", strerror(errno));
                _exit(EXIT_FAILURE);
            }

            _exit(EXIT_SUCCESS);

        default: /* 父进程 */
            printf("[%ld] Parent about to wait for signal\n", (long)getpid());
            /* 等待子进程发送同步信息 */
            sem_op.sem_num = 0;
            sem_op.sem_op = 0;
            sem_op.sem_flg = 0;
            if (semop(semid, &sem_op, 1) == -1) {
                fprintf(stderr, "Parent semop failed: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("[%ld] Parent got signal\n", (long)getpid());
    }

    exit(EXIT_SUCCESS);
}
