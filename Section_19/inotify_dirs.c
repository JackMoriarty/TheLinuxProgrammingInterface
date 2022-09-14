/**
 * @file inotify_dirs.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 监控指定目录下的所有事件, 当事件涉及子目录时, 可自动更新
 * @version 0.1
 * @date 2019-04-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

typedef struct pathname_wd{
    char pathname[PATH_MAX];
    int wd;
    struct pathname_wd *next;
} pathname_wd;

// inotify 实例描述符
int inotify_fd = 0;
pathname_wd pathname_wd_list;

/**
 * @brief ntfw 函数回调函数
 * 
 * @param pathname 文件路径名
 * @param statbuf stat 结构指针
 * @param typeflag 文件相关信息
 * @param ftwbuf FTW 结构指针
 */
static int nftw_func(const char *pathname, const struct stat *statbuf, 
                    int typeflag, struct FTW *ftwbuf)
{
    int retval = 0;

    // 检查当前遍历项的文件类型并添加监视
    if (S_ISDIR(statbuf->st_mode)) {
        retval = inotify_add_watch(inotify_fd, pathname, IN_ALL_EVENTS);
        // 记录
        if (retval != -1) {
            pathname_wd *p = (pathname_wd *)malloc(sizeof(pathname_wd));
            if (p == NULL) {
                perror("ERROR: malloc pathname_wd");
                exit(EXIT_FAILURE);
            }
            p->wd = retval;
            strcpy(p->pathname, pathname);
            p->next = pathname_wd_list.next;
            pathname_wd_list.next = p;
        } else {
            perror("ERROR: add inotify in nftw func");
            exit(EXIT_FAILURE);
        }
    }

    return retval;
}

/**
 * @brief 打印相关信息, 更新相关结构
 * 
 * @param event struct inotify_event结构指针
 */
static void display_modify_inotify_event(struct inotify_event *event)
{
    // 输出结构体中的信息
    printf("wd = %2d;", event->wd);
    if (event->cookie > 0)
        printf("cookie = %4d;", event->cookie);
    
    printf("mask = ");
    if (event->mask & IN_ACCESS)            printf("IN_ACCESS ");
    if (event->mask & IN_ATTRIB)            printf("IN_ATTRIB ");
    if (event->mask & IN_CLOSE_NOWRITE)     printf("IN_CLOSE_NOWRITE ");
    if (event->mask & IN_CLOSE_WRITE)       printf("IN_CLOSE_WRITE ");
    if (event->mask & IN_CREATE)            printf("IN_CREATE ");
    if (event->mask & IN_DELETE)            printf("IN_DELETE ");
    if (event->mask & IN_DELETE_SELF)       printf("IN_DELETE_SELF ");
    if (event->mask & IN_IGNORED)           printf("IN_IGNORED ");
    if (event->mask & IN_ISDIR)             printf("IN_ISDIR ");
    if (event->mask & IN_MODIFY)            printf("IN_MODIFY ");
    if (event->mask & IN_MOVE_SELF)         printf("IN_MOVE_SELF ");
    if (event->mask & IN_MOVED_FROM)        printf("IN_MOVED_FROM ");
    if (event->mask & IN_MOVED_TO)          printf("IN_MOVED_TO ");
    if (event->mask & IN_OPEN)              printf("IN_OPEN ");
    if (event->mask & IN_Q_OVERFLOW)        printf("IN_Q_OVERFLOW ");
    if (event->mask & IN_UNMOUNT)           printf("IN_UNMOUNT ");
    printf("\n");

    if (event->len)
        printf("name = %s\n", event->name);

    // 如果是文件夹, 则对监控项做相应操作
    if (event->mask & IN_ISDIR) {
        // 创建目录
        if (event->mask & IN_CREATE) {
            // 寻找当前监控项路径名称
            pathname_wd *p = pathname_wd_list.next;
            while(p != NULL) {
                if (p->wd == event->wd) break;
                p = p->next;
            }
            if (p == NULL) {
                fprintf(stderr, "ERROR: can not find wd %d\n", event->wd);
                exit(EXIT_FAILURE);
            }

            // 添加监控项并记录
            pathname_wd *new_p = (pathname_wd *)malloc(sizeof(pathname_wd));
            strcpy(new_p->pathname, p->pathname);
            strcat(new_p->pathname, "/");
            strcat(new_p->pathname, event->name);
            int wd = inotify_add_watch(inotify_fd, new_p->pathname, IN_ALL_EVENTS);
            if (wd == -1) {
                char msg[PATH_MAX + 50];
                snprintf(msg, 100, "ERROR(add %s inotify)", new_p->pathname);
                perror(msg);
                exit(EXIT_FAILURE);
            }
            new_p->wd = wd;
            new_p->next = pathname_wd_list.next;
            pathname_wd_list.next = new_p;
        } else if (event->mask & IN_DELETE_SELF) {
            // 删除目录
            // 寻找当前监控项路径名称
            pathname_wd *p = pathname_wd_list.next;
            pathname_wd *tmp_p = NULL;
            while(p != NULL) {
                if (p->wd == event->wd) break;
                tmp_p = p;
                p = p->next;
            }
            if (p == NULL) {
                fprintf(stderr, "ERROR: can not find wd %d\n", event->wd);
                exit(EXIT_FAILURE);
            }

            // 去除监控项和记录
            tmp_p->next = p->next;
            free(p);
            p = NULL;
            if (inotify_rm_watch(inotify_fd, event->wd)) {
                char msg[100];
                snprintf(msg, 100, "ERROR(rm inotify %d)", event->wd);
                perror(msg);
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    ssize_t read_num = 0;
    char buf[BUF_LEN];
    char *p = NULL;
    struct inotify_event *event;

    pathname_wd_list.next = NULL;

    // 参数检查
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filepath\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 创建inotify实例
    inotify_fd = inotify_init();

    // 遍历文件树
    if (nftw(argv[1], nftw_func, 10, 0) == -1) {
        perror("ERROR: nftw");
        exit(EXIT_FAILURE);
    }

    // 获取inotify事件
    while (1) {
        read_num = read(inotify_fd, buf, BUF_LEN);
        if (read_num == 0) {
            fprintf(stderr, "ERROR: we get no data!\n");
            exit(EXIT_FAILURE);
        }
        if (read_num == -1) {
            perror("ERROR: read inotify fd");
            exit(EXIT_FAILURE);
        }
        printf("we get %ld byte from inotify fd\n", (long)read_num);

        // 对缓冲区的多个数据进行处理
        for (p = buf; p < buf + read_num;) {
            event = (struct inotify_event *)p;
            display_modify_inotify_event(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    return 0;
}