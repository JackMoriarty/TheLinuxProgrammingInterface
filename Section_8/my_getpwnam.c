/**
 * @file my_getpwnam.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用setpwent(), getpwent() 和 endpwent()来实现getpwnam()
 * @version 0.1
 * @date 2019-01-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <pwd.h>
#include <string.h>

/**
 * @brief 自己实现的getpwnam
 * 
 * @param name 用户名
 * @return struct passwd* 相关信息结构指针
 */
struct passwd *my_getpwnam(const char *name)
{
    struct passwd *pwd;
    while ((pwd = getpwent()) != NULL) {
        if (!strcmp(pwd->pw_name, name)) {
            return pwd;
        }
    }

    return NULL;
}

int main()
{
    printf("%ld\n", (long)(my_getpwnam("chenbangduo")->pw_uid));
    return 0;
}