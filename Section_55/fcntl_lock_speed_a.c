/**
 * @file fcntl_lock_speed_a.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 锁定指定文件的指定字节范围
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd;
    int i;
    struct flock fl;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, 100000) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_len = 1;

    for (i = 0; i < 40001; i++) {
        fl.l_start = i * 2;
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
    }

    printf("lock success\n");
    pause();
    
    exit(EXIT_SUCCESS);
}