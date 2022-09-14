/**
 * @file mygetlogin.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现getlogin的功能
 * @version 0.1
 * @date 2019-06-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>

char *my_getlogin(void)
{
    struct utmpx *get_utmpx;
    struct utmpx search_utmpx;
    char *tty_path;
    char *tty_name_buffer;

    /* 获取ttyname */
    tty_path = ttyname(STDIN_FILENO);
    if (tty_path == NULL) {
        return NULL;
    }

#if (defined MY_LOGIN_DEBUG) || (defined DEBUG)
    printf("tty_name :%s\n", tty_path);
#endif

    /* 设置utmpx起始位置 */
    setutxent();

    tty_name_buffer = strchr(&tty_path[1], '/');
    if (tty_name_buffer == NULL) {
        return NULL;
    }
    tty_name_buffer += 1;
#if (defined MY_LOGIN_DEBUG) || (defined DEBUG)
    printf("tty_name :%s\n", tty_name_buffer);
#endif

    strcpy(search_utmpx.ut_line, tty_name_buffer);
    /* 搜索指定项 */
    get_utmpx = getutxline(&search_utmpx);
    if (get_utmpx == NULL) {
        return NULL;
    }
    /* 获得对应项目 */
    return get_utmpx->ut_user;
}

int main(int argc, char *argv[])
{
    char *user;

    user = my_getlogin();
    if (user == NULL) {
        perror("my_getlogin");
        exit(EXIT_FAILURE);
    }
    printf("user: %s\n", user);
    exit(EXIT_SUCCESS);
}