/**
 * @file utmpx_login.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 更新utmp,wtmp和lastlog文件
 * @version 0.1
 * @date 2019-06-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmpx.h>
#include <paths.h>
#include <string.h>
#include <time.h>
#include <lastlog.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
    struct utmpx ut;
    struct lastlog llog;
    struct passwd *pwd;
    time_t now_time;
    char *dev_name;
    uid_t uid;
    int fd;

    /* 参数检查 */
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s username\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 为三个文件进行相关结构体初始化 */
    memset(&ut, 0, sizeof(struct utmpx));
    memset(&llog, 0, sizeof(struct lastlog));
    ut.ut_type = USER_PROCESS;
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
    if (time(&now_time) == -1) {
        perror("time");
        exit(EXIT_FAILURE);
    }
    ut.ut_tv.tv_sec = now_time;
    ut.ut_pid = getpid();
    /* 设置`ut_line`和`ut_id`, 本处假定终端名以`/dev/[pt]t[sy]*`形式给出 */
    dev_name = ttyname(STDIN_FILENO);
    if (dev_name == NULL) {
        perror("ttyname");
        exit(EXIT_FAILURE);
    }
    if (strlen(dev_name) <= 8) {
        /* 应该不会发生 */
        fprintf(stderr, "dev_name <= 8\n");
        exit(EXIT_FAILURE);
    }
    strncpy(ut.ut_line, dev_name + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, dev_name + 8, sizeof(ut.ut_id));

    llog.ll_time = now_time;
    strncpy(llog.ll_line, dev_name + 5, sizeof(llog.ll_line));
    /* 在本地进行模拟,因此不设置`llhost`项 */
    llog.ll_host[0] = '\0';
    /* 获取用户id */
    pwd = getpwnam(argv[1]);
    if (pwd == NULL) {
        perror("getpwnam");
        exit(EXIT_FAILURE);
    }
    uid = pwd->pw_uid;

    /* 更新相关文件 */
    /* 更新utmp文件 */
    setutxent();
    if (pututxline(&ut) == NULL) {
        perror("pututxline");
        exit(EXIT_FAILURE);
    }
    /* 更新wtmp文件 */
    updwtmpx(_PATH_WTMPX, &ut);
    /* 更新lastlog */
    fd = open(_PATH_LASTLOG, O_WRONLY);
    if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1) {
        perror("lseek");
        exit(EXIT_FAILURE);
    }
    if (write(fd, &llog, sizeof(struct lastlog)) < 0) {
        perror("write lastlog");
        exit(EXIT_FAILURE);
    }

    /* sleep for a while */
    sleep(5);

    /* 模拟退出 */
    ut.ut_type = DEAD_PROCESS;
    time((time_t *)&ut.ut_tv.tv_sec);
    memset(&ut.ut_user, 0, sizeof(ut.ut_user));

    setutxent();
    if (pututxline(&ut) == NULL) {
        perror("logout pututxline");
        exit(EXIT_FAILURE);
    }
    updwtmpx(_PATH_WTMP, &ut);

    endutxent();

    exit(EXIT_SUCCESS);
}