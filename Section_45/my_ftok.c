/**
 * @file my_ftok.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 实现ftok函数
 * @version 0.1
 * @date 2019-06-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sysmacros.h>

key_t my_ftok(const char *pathname, int proj_d)
{
    struct stat buf;
    long dev_min;
    long inode_id;
    key_t key;

    /* 获取文件信息 */
    if (stat(pathname, &buf) == -1) {
        return -1;
    }

    dev_min = minor(buf.st_dev);
    inode_id = buf.st_ino;

    memset(&key, 0, sizeof(key_t));

    key = inode_id & 0xFFFF;
    key |= ((dev_min & 0xFF) << 16);
    key |= ((proj_d & 0xFF) << 24);

    return key;
}

int main(int argc, char *argv[])
{

    printf("0x%x\n", my_ftok(argv[1], 1));
    printf("0x%x\n", ftok(argv[1], 1));
    exit(EXIT_SUCCESS);
}