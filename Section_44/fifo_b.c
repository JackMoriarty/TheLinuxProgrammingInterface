/**
 * @file fifo_b.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 验证fifo非阻塞
 * @version 0.1
 * @date 2019-06-18
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
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd;

    // /* 读取 */
    // fd = open("local_fifo", O_RDONLY);
    // if (fd == -1) {
    //     perror("阻塞读取");
    //     exit(EXIT_FAILURE);
    // } else {
    //     printf("阻塞读取");
    //     exit(EXIT_SUCCESS);
    // }
    
    // /* 写入*/
    // fd = open("local_fifo", O_WRONLY);
    // if (fd == -1) {
    //     perror("阻塞写入");
    //     exit(EXIT_FAILURE);
    // } else {
    //     printf("阻塞写入");
    //     exit(EXIT_SUCCESS);
    // }

    exit(EXIT_FAILURE);
}