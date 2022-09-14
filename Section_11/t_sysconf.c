/**
 * @file t_sysconf.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用sysconf函数
 * @version 0.1
 * @date 2019-01-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/**
 * @brief 打印相关限制
 * 
 * @param msg 相关消息
 * @param name 限制值名称变量
 */
static void  sysconf_print(const char *msg, int name)
{
    long lim;

    errno = 0;
    lim = sysconf(name);
    if (lim != -1) {
        // 调用成功
        printf("%s %ld\n", msg, lim);
    } else {
        if (errno == 0) {
            // 调用成功, 对应限制值未定义
            printf("%s (indeterminate)\n", msg);
        } else {
            fprintf(stderr, "sysconf %s", msg);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    sysconf_print("_SC_ARG_MAX:             ", _SC_ARG_MAX);
    sysconf_print("_SC_LOGIN_NAME_MAX:      ", _SC_LOGIN_NAME_MAX);
    sysconf_print("_SC_OPEN_MAX:            ", _SC_OPEN_MAX);
    sysconf_print("_SC_NGROUPS_MAX:         ", _SC_NGROUPS_MAX);
    sysconf_print("_SC_PAGESIZE:            ", _SC_PAGESIZE);
    sysconf_print("_SC_RTSIG_MAX:           ", _SC_RTSIG_MAX);
    
    return 0;
}