/**
 * @file mydup.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 用fcntl和close实现dup
 * @version 0.1
 * @date 2018-12-15
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @brief 复制文件描述符
 * 
 * @param oldfd 待复制的文件描述符
 * @return int  复制得到的文件描述符
 */

int mydup(int oldfd)
{
    int newfd;
    newfd = fcntl(oldfd, F_DUPFD, 0);
    return newfd;
}

int main(int argc, char *argv[])
{
    int oldfd, newfd;
    /* 打开(如果没有则创建)文件 */
    oldfd = open("TestFile", O_RDWR|O_CREAT, S_IWUSR|S_IRUSR);
    if(oldfd == -1) {
        fprintf(stderr, "Fail to open file\n");
        exit(EXIT_FAILURE);
    }
    /* 调用自己重写的dup */
    newfd = mydup(oldfd);
    if(newfd == -1) {
        fprintf(stderr, "Fail to copy file-descriptor\n");
        exit(EXIT_FAILURE);
    }
    printf("Success with oldfd(%d) and newfd(%d)\n", oldfd, newfd);
    exit(EXIT_SUCCESS);
}