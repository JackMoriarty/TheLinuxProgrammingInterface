/**
 * @file orphan_process_read.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 孤儿进程组的进程调用read
 * @version 0.1
 * @date 2019-05-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int child_id;
    char buf[512];
    switch (child_id = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        /* wait for parent process exit */
        sleep(5);
        printf("child process wake up\n");
        if (read(STDIN_FILENO, buf, 512) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("read success\n");
        break;
    
    default:
        printf("parent process exit\n");
        break;
    }
    exit(EXIT_SUCCESS);
}