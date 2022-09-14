/**
 * @file flock_dup.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 测试复制文件描述符对flock锁的影响
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, dup_fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock fd");
        exit(EXIT_FAILURE);
    }

    if ((dup_fd = dup(fd)) == -1) {
        perror("dup fd");
        exit(EXIT_FAILURE);
    }
    if (flock(dup_fd, LOCK_UN) == -1) {
        perror("flock dupfd release");
        exit(EXIT_FAILURE);
    }

    pause();
    exit(EXIT_FAILURE);
}