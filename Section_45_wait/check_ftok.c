/**
 * @file check_ftok.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 验证ftok函数
 * @version 0.1
 * @date 2019-06-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/sysmacros.h>

int main(int argc, char *argv[])
{
    struct stat buf;
    int proj = 1;
    int key;

    /* 参数检查 */
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 获取文件信息 */
    if (stat(argv[1], &buf) == -1) {
        fprintf(stderr, "stat %s failed: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* 输出相关信息 */
    printf("0x%x 0x%x 0x%x\n", (unsigned int)buf.st_ino, minor(buf.st_dev), proj);
    key = ftok(argv[1], 1);
    printf("0x%x\n", key);

    exit(EXIT_SUCCESS);
}