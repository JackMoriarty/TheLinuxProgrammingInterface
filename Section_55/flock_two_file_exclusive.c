/**
 * @file flock_two_file_exclusive.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用排它锁锁定两个文件
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd1, fd2;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename1 filename2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 打开两个文件 */
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        perror("open file1");
        exit(EXIT_FAILURE);
    }
    fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1) {
        perror("open file2");
        exit(EXIT_FAILURE);
    }

    /* 锁定第一个文件 */
    if (flock(fd1, LOCK_EX) == -1) {
        perror("flock fd1");
        exit(EXIT_FAILURE);
    }
    printf("lock file1\n");

    /* sleep for 10s */
    sleep(10);

    /* 锁定第二个文件 */
    if (flock(fd2, LOCK_EX) == -1) {
        perror("flock fd2");
        exit(EXIT_FAILURE);
    }
    printf("lock file2\n");

    /* sleep for 10s */
    sleep(10);

    /* 释放两个锁 */
    if (flock(fd1, LOCK_UN) == -1) {
        perror("flock fd1 release");
        exit(EXIT_FAILURE);
    }
    printf("release file1\n");
    if (flock(fd2, LOCK_UN) == -1) {
        perror("flock fd2 release");
        exit(EXIT_FAILURE);
    }
    printf("release file2\n");

    /* 关闭两个文件 */
    close(fd1);
    close(fd2);

    exit(EXIT_SUCCESS);
}