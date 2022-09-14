/**
 * @file t_fpathconf.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用fpathconf函数
 * @version 0.1
 * @date 2019-01-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief fpathconf 相关信息输出
 * 
 * @param msg 要输出的信息前缀
 * @param fd 对应的文件描述符
 * @param name 限制值名称整数值
 */
static void fpathconf_print(const char *msg, int fd, int name)
{
    long lim;
    
    errno = 0;
    lim = fpathconf(fd, name);
    if (lim != -1) {
        // 调用成功
        printf("%s %ld\n", msg, lim);
    } else {
        if (errno == 0) {
            // 调用成功, 无对应限制值
            printf("%s (indeterminate)\n", msg);
        } else {
            // 调用失败
            fprintf(stderr, "fpathconf %s\n", msg);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    fpathconf_print("_PC_NAME_MAX:  ", STDIN_FILENO, _PC_NAME_MAX);
    fpathconf_print("_PC_PATH_MAX:  ", STDIN_FILENO, _PC_PATH_MAX);
    fpathconf_print("_PC_PIPE_BUF:  ", STDIN_FILENO, _PC_PIPE_BUF);
    
    return 0;
}