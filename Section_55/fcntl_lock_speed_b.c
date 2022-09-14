/**
 * @file fcntl_lock_speed_b.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 尝试锁定指定文件的指定字节范围
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
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd;
    int byte_index;
    int i;
    struct flock fl;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename times\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    byte_index = atoi(argv[2]);

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_len = 1;
    fl.l_start = byte_index * 2;

    for (i = 0; i < 10000; i++) {
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            if (errno == EACCES || errno == EAGAIN) {
                continue;
            } else {
                perror("fcntl");
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}