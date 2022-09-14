/**
 * @file check_fd.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 
 * @version 0.1
 * @date 2018-12-16
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int oldfd, newfd;
    /* 打开(如果没有则创建)文件 */
    oldfd = open("TestFile", O_RDWR|O_CREAT, S_IWUSR|S_IRUSR);
    if(oldfd == -1) {
        fprintf(stderr, "Fail to open file\n");
        exit(EXIT_FAILURE);
    }
    /* 复制文件描述符 */
    newfd = dup(oldfd);
    if(newfd == -1) {
        fprintf(stderr, "Fail to copy file-descriptor\n");
        exit(EXIT_FAILURE);
    }
    printf("Success with oldfd(%d) and newfd(%d)\n", oldfd, newfd);
    /* 设置oldfd的标志, 获取newfd的标志 */
    if(fcntl(oldfd, F_SETFL, O_RDWR|O_APPEND|O_CREAT|O_ASYNC) == -1) {
        fprintf(stderr, "Fail to set file-descriptor flags\n");
        exit(EXIT_FAILURE);
    }
    printf("oldfd flags: %d\n", fcntl(oldfd, F_GETFL));
    printf("newfd flags: %d\n", fcntl(newfd, F_GETFL));
    /* 移动oldfd偏移量 */
    printf("oldfd offset:%lld and newfd offset:%lld\n", (long long)lseek(oldfd, 0, SEEK_CUR), 
            (long long)lseek(newfd, 0, SEEK_CUR));
    printf("oldfd newoffset:%lld\n",(long long)lseek(oldfd, 100, SEEK_CUR));
    printf("newfd offset %lld\n", (long long)lseek(newfd, 0, SEEK_CUR));
    exit(EXIT_SUCCESS);
}