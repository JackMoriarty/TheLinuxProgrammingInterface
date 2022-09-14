/**
 * @file vfork.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 验证vfork函数对父进程文件描述符没有影响
 * @version 0.1
 * @date 2019-04-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    switch (vfork()) {
    case -1:
        perror("vfork");
        exit(EXIT_FAILURE);

    case 0: /* 子进程 */
        /* 关闭 0 号文件描述符 */
        if (close(0) == -1) {
            perror("close in child process");
            _exit(EXIT_FAILURE);
        }
        printf("child process rm 0 fd success\n");
        _exit(EXIT_SUCCESS);
    
    default: /* 父进程 */
        /* 尝试关闭 0 号文件描述符 */
        sleep(5);
        if (close(0) == -1 && errno == EBADF) {
            perror("close in parent process");
            exit(EXIT_FAILURE);
        }

        printf("close success, fd exist\n");
        exit(EXIT_SUCCESS);
    }
}