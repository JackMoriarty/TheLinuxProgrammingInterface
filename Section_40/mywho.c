/**
 * @file mywho.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 简单的who(1)
 * @version 0.1
 * @date 2019-06-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>
#include <time.h>

#define OPTION_ARRAY_SIZE   10
#define ERR_MSG_SIZE        1024

extern char *optarg;
extern int optind, optopt;

enum {
    b,
    d,
    l,
    p,
    r,
    t,
    T,
    H,
    u,
    q
};
enum {
    false,
    true
};

int main(int argc, char *argv[])
{
    int opt;
    int i;
    int sum;
    char option[OPTION_ARRAY_SIZE];
    char *new_path;
    char user_name[__UT_NAMESIZE];
    char err_msg[ERR_MSG_SIZE];
    struct utmpx *utx;

    memset(option, false, sizeof(char) * OPTION_ARRAY_SIZE);

    /* 选项解析 */
    while ((opt = getopt(argc, argv, ":abdlprtTHu:q")) != -1) {
        if (opt == '?' || opt == ':') {
            printf(";optopt = %c\n", optopt);
        }
        
        switch(opt) {
            case 'a':
                for (i = 0; i < OPTION_ARRAY_SIZE; i++) {
                    if (i == u) continue;
                    option[i] = 1;
                }
                break;
            case 'b':
                option[b] = true;
                break;
            case 'd':
                option[d] = true;
                break;
            case 'l':
                option[l] = true;
                break;
            case 'p':
                option[p] = true;
                break;
            case 'r':
                option[r] = true;
                break;
            case 't':
                option[t] = true;
                break;
            case 'T':
                option[T] = true;
                break;
            case 'H':
                option[H] = true;
                break;
            case 'u':
                option[u] = true;
                strncpy(user_name, optarg, __UT_NAMESIZE * sizeof(char));
                break;
            case 'q':
                option[q] = true;
                break;

            default:
                fprintf(stderr, "unexcept case in switch!\n");
                exit(EXIT_FAILURE);
        }
    }

    /* 是否使用默认路径 */
    if (optind < argc) {
        if (utmpxname(argv[optind]) == -1) {
            snprintf(err_msg, ERR_MSG_SIZE * sizeof(char), 
                "change path(%s) failed, use default path:", argv[optind]);
        }
    }

    /* 输出头 */
    if (option[H]) {
        printf("USER             LINE             ID               "
                "HOST                       ");
        if (option[l])
            printf("PID             ");
        if (option[t])
            printf("TIME\n");
    }

    /* 检索文件 */
    setutxent();
    sum = 0;
    while ((utx = getutxent()) != NULL) {
        /* 仅显示指定用户 */
        if (option[u] && strcmp(utx->ut_user, user_name)) continue;
        /* 不显示启动信息 */
        if (option[b] == false && utx->ut_type == BOOT_TIME) continue;
        /* 不显示登出信息 */
        if (option[d] == false && utx->ut_type == DEAD_PROCESS) continue;
        /* 不打印init创建的进程 */
        if (option[p] == false && utx->ut_type == INIT_PROCESS) continue;
        /* 不打印运行级别改变 */
        if (option[r] == false && utx->ut_type == RUN_LVL) continue;
        /* 用户名 终端名 终端ID 主机名*/
        printf("%-16s %-16s %-16s %-27s",
             utx->ut_user, utx->ut_line, utx->ut_id, utx->ut_host);
        /* 进程ID */
        if (option[l])
            printf("%-16ld", (long)utx->ut_pid);
        /* 上次系统时钟改变 */
        if (option[t] && (utx->ut_type == OLD_TIME || utx->ut_type == NEW_TIME))
            printf("%-16s", ctime((time_t *)&utx->ut_tv.tv_sec));
        printf("\n");
        sum++;
    }

    if (option[q])
        printf("# user=%d\n", sum);

    exit(EXIT_SUCCESS);
}