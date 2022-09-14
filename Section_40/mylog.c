/**
 * @file mylog.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现`login` `logout` 和 `logwtmp`函数
 * @version 0.1
 * @date 2019-06-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utmp.h>
#include <unistd.h>

/**
 * @brief 登陆
 * 
 * @param ut `utmp`结构体
 */
void my_login(const struct utmp *ut)
{
    /* 设置`utmp`当前位置为文件起始位置 */
    setutent();
    /* 更新`utmp`文件 */
    if (pututline(ut) == NULL) {
        perror("my_login write to utmp:");
        exit(EXIT_FAILURE);
    }
    /* 更新`wtmp`文件 */
    updwtmp(_PATH_WTMP, ut);
    /* 关闭打开的`utmp` */
    endutent();
}

/**
 * @brief 登出
 * 
 * @param ut_line 终端名
 * @return int 1表示成功, 0表示失败
 */
int my_logout(const char *ut_line)
{
    struct utmp ut;
    struct utmp *ut_log;
    /* 设置`utmp`当前位置为文件起始位置 */
    setutent();
    strncpy(ut.ut_line, ut_line, sizeof(ut.ut_line));
    ut_log = getutline(&ut);
    if (ut_log == NULL) {
        return 0;
    }
    memcpy(&ut, ut_log, sizeof(struct utmp));
    ut.ut_type = DEAD_PROCESS;
    if (time((time_t *)&ut.ut_tv.tv_sec) == -1) {
        return 0;
    }
    /* 更新`utmp`文件 */
    if (pututline(&ut) == NULL) {
        return 0;
    }
    /* 关闭打开的`utmp` */
    endutent();
    return 1;
}

/**
 * @brief 追加记录到wtmp文件
 * 
 * @param line 终端名
 * @param name 用户名
 * @param host 远程登陆主机名
 */
void my_logwtmp(const char *line, const char *name, const char *host)
{
    struct utmp ut;
    memset(&ut, 0, sizeof(struct utmp));
    
    strncpy(ut.ut_line, line, sizeof(ut.ut_line));
    strncpy(ut.ut_user, name, sizeof(ut.ut_user));
    strncpy(ut.ut_host, host, sizeof(ut.ut_host));
    if (time((time_t *)&ut.ut_tv.tv_sec) == -1) {
        perror("my_logwtmp set time:");
        exit(EXIT_FAILURE);
    }
    ut.ut_pid = getpid();

    /* 更新`wtmp`文件 */
    updwtmp(_PATH_WTMP, &ut);
}

int main(int argc, char *argv[])
{
    struct utmp ut;
    char *devName;

    memset(&ut, 0, sizeof(struct utmp));
    ut.ut_type = USER_PROCESS;
    strncpy(ut.ut_user, "root", sizeof(ut.ut_user));
    if (time((time_t *)&ut.ut_tv.tv_sec) == -1) {
        perror("time");
        exit(EXIT_FAILURE);
    }
    ut.ut_pid = getpid();
    devName = ttyname(STDIN_FILENO);
    if (devName == NULL) {
        perror("ttyname");
        exit(EXIT_FAILURE);
    }
    if (strlen(devName) < 8) {
        perror("strlen");
        exit(EXIT_FAILURE);
    }
    strncpy(ut.ut_line, devName + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, devName + 8, sizeof(ut.ut_id));
    
    my_login(&ut);
    printf("my_login\n");
    getchar();
    my_logout(ut.ut_line);
    printf("my_logout\n");
    getchar();
    my_logwtmp(ut.ut_line, ut.ut_user, "remote-host");
    printf("my_logwtmp");
    getchar();
    exit(EXIT_SUCCESS);
}