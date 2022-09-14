/**
 * @file mydup2.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 用fnctl和close实现dup2()
 * @version 0.1
 * @date 2018-12-16
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief dup2 个人实现
 * 
 * @param oldfd 待复制的文件描述符
 * @param newfd 希望得到的文件描述符
 * @return int  得到的描述符
 */
int mydup2(int oldfd, int newfd)
{
    int fd;
    /* 检查newfd是否已经被打开 */
    if(fcntl(newfd, F_GETFL) >= 0) {
        close(newfd);
    }
    /* 检查oldfd与newfd是否相同并且有效 */
    if(oldfd == newfd && fcntl(oldfd, F_GETFL) >= 0) {
        errno = EBADF;
        return -1;
    }
    fd = fcntl(oldfd, F_DUPFD, newfd);
    return fd;
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
    newfd = mydup2(oldfd, 25);
    if(newfd == -1) {
        fprintf(stderr, "Fail to copy file-descriptor\n");
        exit(EXIT_FAILURE);
    }
    printf("Success with oldfd(%d) and newfd(%d)\n", oldfd, newfd);
    exit(EXIT_SUCCESS);
}