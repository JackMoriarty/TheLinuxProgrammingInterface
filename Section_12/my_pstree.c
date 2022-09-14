/**
 * @file my_pstree.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 实现一个pstree
 * @version 0.1
 * @date 2019-01-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

/* 进程树节点 */
struct pid_tree_node {
    char program_name[FILENAME_MAX];    // 进程名
    pid_t pid;  // 进程号
    struct pid_tree_node *next;
};

/* 具有相同父进程节点列表头 */
struct ppid_head {
    pid_t ppid;
    struct pid_tree_node *pid_next;
    struct ppid_head *next;
};

/* 具有相同父进程列表集合头 */
struct ppid_head ppid_list_head;

/**
 * @brief 生成进程树(将指定的pid添加到对应列表中)
 * 
 * @param fp 对应PID目录status文件指针
 */
void generate_pid_tree(FILE *fp)
{
    char buffer[BUFFER_SIZE];
    char program_name[NAME_MAX];
    pid_t pid;
    pid_t ppid;
    char *name = NULL;
    char *value = NULL;
    struct pid_tree_node *ptn = NULL;
    struct ppid_head *plh = NULL;

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        // 除去buffer中末尾的换行符
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        
        // 查找所需项目的行数
        name = strtok(buffer, ":");

        if (name != NULL && strcmp(name, "Name") == 0) {
            // 获取程序名
            while ((value = strtok(NULL, "\t")) != NULL && *value == '\0');
            if (value != NULL) {
                strcpy(program_name, value);
            }
        } else if (name != NULL && strcmp(name, "Pid") == 0) {
            // 获取进程号
            while ((value = strtok(NULL, "\t")) != NULL && *value == '\0');
            if (value != NULL) {
                pid = strtol(value, NULL, 10);
            }
        } else if (name != NULL && strcmp(name, "PPid") == 0) {
            // 获取父进程号

            while ((value = strtok(NULL, "\t")) != NULL && *value == '\0');
            if (value != NULL) {
                ppid = strtol(value, NULL, 10);
                if (value != NULL) {
                    ppid = strtol(value, NULL, 10);
                }
            }
        }
    }
        // 生成节点, 并添加到指定父进程列表中
        ptn = (struct pid_tree_node *)malloc(sizeof(struct pid_tree_node));
        strcpy(ptn->program_name, program_name);
        ptn->pid = pid;
        plh = ppid_list_head.next;

        for (; plh != NULL && plh->ppid != ppid; plh = plh->next);
        if (plh == NULL) {
            // 当前父进程还未在列表中
            plh = (struct ppid_head *)malloc(sizeof(struct ppid_head));
            plh->ppid = ppid;
            plh->pid_next = ptn;
            ptn->next = NULL;
            plh->next = ppid_list_head.next;
            ppid_list_head.next = plh;
        } else {
            // 当前父进程已经在列表中
            ptn->next = plh->pid_next;
            plh->pid_next = ptn;
        }
}

/**
 * @brief 深度优先递归打印
 * 
 * @param hp 特定父进程号列表头
 */
void print_pid_dfs(struct ppid_head *hp, int deepth)
{
    struct pid_tree_node *ptn = NULL;
    struct ppid_head *plh = NULL;

    if (hp == NULL) {
        putchar('\n');
        return;
    }

    // 深度优先遍历
    for (ptn = hp->pid_next; ptn != NULL; ptn = ptn->next) {
        printf("%-11s", ptn->program_name);

        // 找到此进程所有子进程
        plh = ppid_list_head.next;
        for (; plh != NULL && plh->ppid != ptn->pid; plh = plh->next);
        print_pid_dfs(plh, deepth + 1);
       
        // 制表控制
        if (ptn->next != NULL) {
            for (int i = 0; i < deepth - 1; i++) {
                printf("%11s", "     |     ");
            }
            printf("%11s", "     |-----");
        }
    }
}
/**
 * @brief 打印进程树
 * 
 */
void print_pid_tree()
{
    struct ppid_head *plh = ppid_list_head.next, *plh_tmp = NULL;
    struct pid_tree_node *ptn = NULL, *ptn_tmp = NULL;

    // 找到父进程号为0的列表
    for (; plh != NULL && plh->ppid != 0; plh = plh->next);
    // 深度优先, 递归打印
    print_pid_dfs(plh, 0);

    // 清理生成的进程树
    while (plh != NULL) {
        ptn = plh->pid_next;
        plh_tmp = plh->next;
        while (ptn != NULL) {
            ptn_tmp = ptn->next;
            free(ptn);
            ptn = ptn_tmp;
        }
        free(plh);
        plh = plh_tmp;
    }
}

/**
 * @brief 遍历`/proc`目录
 * 
 */
void traverse_pid_tree()
{
    struct dirent * dir_p = NULL;
    DIR *dir_s = NULL;
    char file_name[270];
    int file_name_size = 0;
    FILE *fp = NULL;

    dir_s = opendir("/proc");
    if (dir_s == NULL) {
        fprintf(stderr, "can not open dir `/proc`!\n");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    while ((dir_p = readdir(dir_s)) != NULL) {
        errno = 0;
        file_name_size = 0;

        if ((dir_p->d_name)[0] >='0' && (dir_p->d_name)[0] <='9') {
            // 拼接路径名
            strcpy(file_name, "/proc/");
            file_name_size += strlen("/proc/");
            strcpy(file_name + file_name_size, dir_p->d_name);
            file_name_size += strlen(dir_p->d_name);
            strcpy(file_name + file_name_size, "/status");
            
            // 打开指定文件
            fp = fopen(file_name, "r");
            if(fp != NULL) {
                // 生成进程树
                generate_pid_tree(fp);
                fclose(fp);
            }
        }
    }
    if (errno != 0) {
        fprintf(stderr, "something error in readdir()!\n");
        exit(EXIT_FAILURE);
    }

    closedir(dir_s);
}

void print_tree() {
    struct ppid_head *plh = NULL;
    struct pid_tree_node *ptn = NULL;
    
    for (plh = ppid_list_head.next; plh != NULL; plh = plh->next) {
        printf("%ld:", (long)plh->ppid);
        for (ptn = plh->pid_next; ptn != NULL; ptn = ptn->next) {
            printf("%15s(%ld)", ptn->program_name, (long)ptn->pid);
        }
        putchar('\n');
    }
}
int main(int argc, char *argv[])
{
    ppid_list_head.next = NULL;
    traverse_pid_tree();
    // 打印进程树
    print_pid_tree();
    // print_tree();
    return 0;
}