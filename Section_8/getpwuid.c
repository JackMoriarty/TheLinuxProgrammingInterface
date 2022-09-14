/**
 * @file getpwnam.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 证实getpwnam是不可重入函数
 * @version 0.1
 * @date 2018-12-31
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <pwd.h>

int main()
{
    printf("%s %s\n", getpwuid(0)->pw_name, getpwuid(1000)->pw_name);
    return 0;
}