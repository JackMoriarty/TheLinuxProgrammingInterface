/**
 * @file my_setenv_unsetenv.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 自己实现setenv和unsetenv
 * @version 0.1
 * @date 2018-12-30
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

/**
 * @brief 自己实现的setenv函数
 * 
 * @param name 变量名
 * @param value 变量值
 * @param overwrite 是否覆盖(非0为覆盖, 0为不覆盖)
 * @return int 0 表示成功, -1表示失败
 */
int my_setenv(const char *name, const char *value, int overwrite)
{
    int name_len = strlen(name);
    int value_len = strlen(value);
    int string_len = name_len + value_len + 2;
    char *string = (char *)malloc(string_len);
    if (string == NULL) {
        printf("Fail to malloc environment string!\n");
        return -1;
    }

    // 拼接字符串
    strcpy(string, name);
    string[name_len] = '=';
    strcpy(&(string[name_len + 1]), value);

    printf("String is %s\n", string);

    // 提交到环境变量列表中
    if(getenv(name) != NULL && overwrite == 0) {
        printf("do not change %s\n", name);
        return 0;
    }
    if (putenv(string)) {
        printf("Fail to add %s\n", string);
        return -1;
    }
    return 0;
}

/**
 * @brief 自己实现的unsetenv
 * 
 * @param name 变量名
 * @return int 0 标识成功, -1表示失败
 */
int my_unsetenv(const char *name)
{
    char **ep;
    int name_len = strlen(name);

    if (environ == NULL) {
        return -1;
    }
    for (ep = environ; *ep != NULL; ep++) {
        if (strncmp(*ep, name, name_len) == 0) {
            // **ep = '\0';
            for(char **tmp = ep; *tmp != NULL; tmp++) {
                *tmp = *(tmp + 1);
            }
            continue;
        }
    }
    return 0;
}

void print_env() {
    char **ep;
    for (ep = environ; *ep != NULL; ep++) {
        printf("%s\n", *ep);
    }
}

int main()
{
    print_env();
    printf("-----------------\n");
    my_setenv("MyName", "Bangduo Chen", 0);
    print_env();
    printf("-----------------\n");
    my_setenv("MyName", "Jack Moriarty", 1);
    print_env();
    printf("-----------------\n");
    my_unsetenv("MyName");
    print_env();
    printf("-----------------\n");
    return 0;
}