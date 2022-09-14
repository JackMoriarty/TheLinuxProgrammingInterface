/**
 * @file my_getcwd.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 获取工作目录
 * @version 0.1
 * @date 2019-02-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct path_name {
    char name[NAME_MAX];
    struct path_name *next;
} path_name;

void free_list(path_name *path_list)
{
    path_name *tmp;
    while (path_list != NULL) {
        tmp = path_list;
        path_list = path_list->next;
        free(tmp);
    }
}

char *my_getcwd(char *cwdbuf, size_t size)
{
    path_name path_head, *path_ptr;
    struct stat sondir_stat, fatherdir_stat, fatherson_dir_stat;
    DIR *father_path;
    struct dirent *dp;
    int buf_ind = 0;
    int fd;

    path_head.next = NULL;

    // 记录当前位置
    fd = open(".", S_IRUSR);
    if (fd == -1) {
        perror("record pos");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // 获取当前目录信息
        if (stat(".", &sondir_stat) == -1) {
            // 清理链表
            free_list(path_head.next);     
            return NULL;
        }
        // 获取父目录信息
        if (stat("..", &fatherdir_stat) == -1) {
            // 清理链表     
            free_list(path_head.next); 
            return NULL;
        }

        // 父目录与当前目录信息比较, 决定是否退出
        if (fatherdir_stat.st_dev == sondir_stat.st_dev && 
                fatherdir_stat.st_ino == sondir_stat.st_ino) {
                break;
        }

        // 遍历父文件夹下的项目
        if ((father_path = opendir("..")) == NULL) {
            // 清理链表
            free_list(path_head.next);     
            return NULL;
        }

        // 更改当前工作目录
        if (chdir("..") == -1) {
            // 清理链表
            free_list(path_head.next);      
            return NULL;
        }
        while(1) {
            errno = 0;
            dp = readdir(father_path);
            if (dp == NULL) {
                if (errno != 0) {
                    // 清理链表
                    free_list(path_head.next); 
                    return NULL;
                }
                // 父目录项目已全部遍历
                break;
            }
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                continue;
            }

            // 获取当前项目信息
            if (stat(dp->d_name, &fatherson_dir_stat) == -1) {
                return NULL;
            }

            if (fatherson_dir_stat.st_dev == sondir_stat.st_dev && 
                    fatherson_dir_stat.st_ino == sondir_stat.st_ino) {
                // 从父目录所属项目中找到当前文件夹
                path_ptr = (path_name *)malloc(sizeof(path_name));
                strcpy(path_ptr->name, dp->d_name);
                path_ptr->next = path_head.next;
                path_head.next = path_ptr;
                break;
            }
        }
        if (closedir(father_path) == -1) {
            free_list(path_head.next);
            return NULL;
        }
    }

    // 搜索完毕
    // 生成路径字符串, 清理链表
    for (path_ptr = path_head.next; path_ptr != NULL; ) {
        if (buf_ind + strlen(path_ptr->name + 1) > size) {
            errno = ERANGE;
            // 清理链表
            free_list(path_ptr);
            return NULL;
        }
        strcpy(&cwdbuf[buf_ind], "/");
        strcpy(&cwdbuf[buf_ind + 1], path_ptr->name);
        buf_ind += strlen(path_ptr->name) + 1;
        path_name *tmp = path_ptr;
        path_ptr = path_ptr->next;
        free(tmp);
    }

    // 回到记录的位置
    if (fchdir(fd) == -1) {
        perror("go back");
        exit(EXIT_FAILURE);
    }

    return cwdbuf;
}

int main(int argc, char *argv[])
{
    char cwd[PATH_MAX];
    if (my_getcwd(cwd, PATH_MAX) == NULL) {
        perror("my_getcwd");
        exit(EXIT_FAILURE);
    }
    
    printf("%s\n", cwd);

    return 0;
}