/**
 * @file my_realpath.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 自己实现解析路径名
 * @version 0.1
 * @date 2019-02-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct part_path_name {
    char name[NAME_MAX + 1];
    struct part_path_name *next;
} part_path_name_node;

part_path_name_node *get_part_path(char *pathname, part_path_name_node **ppnn_end)
{
    char *p_ptr = pathname;
    part_path_name_node *ppnn_tmp, ppnn_head;
    int p_index = 0;
    
    ppnn_head.next = NULL;
    *ppnn_end = &ppnn_head;

    if (pathname == NULL) {
        return NULL;
    }

    // 生成部分路径名栈
    for (; ; p_ptr++) {
        if (*p_ptr != '/' && *p_ptr != '\0') {
            if (p_index == 0) {
                ppnn_tmp = (part_path_name_node *)malloc(sizeof(part_path_name_node));
                ppnn_tmp->next = NULL;
            }
            ppnn_tmp->name[p_index++] = *p_ptr;
        } else {
            if (p_index != 0) {
                ppnn_tmp->name[p_index] = '\0';
                (*ppnn_end)->next = ppnn_tmp;
                *ppnn_end = ppnn_tmp;
                // printf("getpartpath:%s\n", ppnn_tmp->name);
            }

            if (*p_ptr == '\0') break;
            
            p_index = 0;
        }
    }

    ppnn_tmp = ppnn_head.next;
    return ppnn_tmp;
}

char *my_realpath(char *pathname, char *resolved_path)
{
    char *local_resolved_path = resolved_path;
    char *lrp_p;
    char *pathname_ptr = pathname;
    char slink_target[PATH_MAX];
    struct stat statbuf;
    part_path_name_node ppnnh, *ppnn_ptr, *ppnn_end, *slink_tar_part_path;

    ppnnh.next = NULL;


    if (pathname == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (strlen(pathname) > PATH_MAX) {
        errno = ENAMETOOLONG;
        return NULL;
    }

    // 当resolved_path为NULL时, 申请一段内存空间
    if (resolved_path == NULL) {
        local_resolved_path = (char *)malloc(PATH_MAX * sizeof(char));
        if (local_resolved_path == NULL) {
            // errno = ENOMEM;
            return NULL;
        }
    }
    local_resolved_path[0] = '/';
    local_resolved_path[1] = '\0';
    lrp_p = local_resolved_path;

    if (pathname[0] != '/') {
        // 相对路径, 先填充工作目录路径
        if (getcwd(local_resolved_path, PATH_MAX) == NULL) {
            return NULL;
        }
        lrp_p = local_resolved_path + strlen(local_resolved_path);
    }

    // 路径解析
    ppnnh.next = get_part_path(pathname, &ppnn_end);
    ppnn_ptr = ppnnh.next;

    while(ppnn_ptr != NULL) {
        ppnnh.next = ppnn_ptr->next;

        // . 的情况无需处理
        if (strcmp("..", ppnn_ptr->name) == 0) {
            // 回滚到父目录
            lrp_p = lrp_p > local_resolved_path + 1 ? lrp_p : local_resolved_path + 1;
            for (; lrp_p > local_resolved_path + 1 && *lrp_p != '/'; lrp_p-- );
            *lrp_p = '\0';
        } else if (strcmp(".", ppnn_ptr->name)){
            *(lrp_p++) = '/';
            strcpy(lrp_p, ppnn_ptr->name);

            // puts(local_resolved_path);
            if (lstat(local_resolved_path, &statbuf) == -1) {
                return NULL;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                lrp_p += strlen(ppnn_ptr->name);
            } else if (S_ISLNK(statbuf.st_mode)) {
                // 该部分为软链接
                // 获取软链接内容
                ssize_t slink_target_size = 0;
                if ((slink_target_size = readlink(local_resolved_path, slink_target, PATH_MAX)) == -1) {
                    return NULL;
                }
                slink_target[slink_target_size] = '\0';
                if (slink_target[0] == '/') {
                    lrp_p = local_resolved_path + 1;
                }

                // 软链接内容路径解析, 并加入链表
                slink_tar_part_path = get_part_path(slink_target, &ppnn_end);
                if (slink_tar_part_path == NULL) {
                    // 此时应为根目录
                    lrp_p = local_resolved_path + 1;
                    *lrp_p = '\0';
                } else {
                    ppnnh.next = slink_tar_part_path;
                    ppnn_end->next = ppnn_ptr->next;
                    lrp_p -= strlen(ppnn_ptr->name);
                    *lrp_p = '\0';
                }
            } else {
                // 该部分为文件, 字符(块)设备,FIFO, 套接字此时判断是否到达解析尾部
                if (ppnn_ptr->next != NULL) {
                    errno = ENOTDIR;
                    return NULL;
                }
            }
        }

        free(ppnn_ptr);
        ppnn_ptr = ppnnh.next;
    }
    return local_resolved_path;
}

int main(int argc, char *argv[])
{
    char *path;
    if (argc != 2) {
        fprintf(stderr, "%s filepath.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    path = my_realpath(argv[1], NULL);
    if (path == NULL){
        perror("my_realpath");
    } else {
        printf("%s\n", path);
    }

    return 0;
}