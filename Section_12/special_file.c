/**
 * @file special_file.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 列表展示打开同意特定路径的所有进程
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
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>

#define BUFFER_SIZE 1024

/**
 * @brief 打印文件中包含的名称和pid
 * 
 * @param fp 文件指针
 */
void print_name_pid(FILE *fp)
{
    char buffer[BUFFER_SIZE];
    char program_name[NAME_MAX];
    pid_t program_pid;
    char *name = NULL;
    char *value = NULL;

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
                program_pid = strtol(value, NULL, 10);
            }
        }
    }

    printf("%-20s\t:%ld\n", program_name, (long)program_pid);
}

/**
 * @brief 遍历`/proc/PID`文件夹
 * 
 * @param filename  要找的文件名称
 */
void traverse_proc_pid(char *filename)
{
    struct dirent *dir_p = NULL;
    DIR *dir_s = NULL;
    char proc_file_name[270];
    int proc_file_name_length = 0;
    char proc_fd_name[PATH_MAX];
    char proc_fd_softlink_name[PATH_MAX];
    int proc_fd_length = 0;
    DIR *proc_fd_s = NULL;
    struct dirent *proc_fd_p = NULL;
    char softlink_target_name[PATH_MAX];
    
    FILE *fp = NULL;

    // 打开`/proc`目录
    dir_s = opendir("/proc");
    if (dir_s == NULL) {
        fprintf(stderr, "can not open dir `/proc`!\n");
        exit(EXIT_FAILURE);
    }

    while ((dir_p = readdir(dir_s)) != NULL) {
        proc_fd_length = 0;

        // 排除非进程文件夹
        if ((dir_p->d_name)[0] >='0' && (dir_p->d_name)[0] <='9') {
            // 拼接`/proc/PID/fd`路径名
            strcpy(proc_fd_name, "/proc/");
            proc_fd_length += strlen("/proc/");
            strcpy(proc_fd_name + proc_fd_length, dir_p->d_name);
            proc_fd_length += strlen(dir_p->d_name);
            strcpy(proc_fd_name + proc_fd_length, "/fd/");
            
            // 打开`/proc/PID/fd`目录
            proc_fd_s = opendir(proc_fd_name);
            if (proc_fd_s == NULL) {
                fprintf(stderr, "can not open dir `/proc/%s/fd`.\n", dir_p->d_name);
            }

            // 遍历`/proc/PID/fd`下的软链接
            proc_fd_length = strlen(proc_fd_name);
            strcpy(proc_fd_softlink_name, proc_fd_name);
            while ((proc_fd_p = readdir(proc_fd_s)) != NULL) {
                
                // 排除`.`和`..`
                if (proc_fd_p->d_name[0] >= '0' && proc_fd_p->d_name[0] <= '9') {
                    // 拼接软链接路径
                    strcpy(proc_fd_softlink_name + proc_fd_length, proc_fd_p->d_name);

                    // 获取软链接真实名称
                    int numbytes = readlink(proc_fd_softlink_name, softlink_target_name, PATH_MAX - 1);
                    softlink_target_name[numbytes] = '\0';
                    // printf("%s-+:+-%s\n", proc_fd_softlink_name, softlink_target_name);
                    // 判断是否与要找的文件名是否相投
                    if (strcmp(softlink_target_name, filename) == 0) {
                        // 匹配, 获取进程名

                        // status 路径拼接
                        proc_file_name_length = 0;
                        strcpy(proc_file_name, "/proc/");
                        proc_file_name_length += strlen("/proc/");
                        strcpy(proc_file_name + proc_file_name_length, dir_p->d_name);
                        proc_file_name_length += strlen(dir_p->d_name);
                        strcpy(proc_file_name + proc_file_name_length, "/status");

                        // 打开指定文件
                        fp = fopen(proc_file_name, "r");
                
                        if(fp != NULL) {
                            print_name_pid(fp);
                            fclose(fp);
                        }
                        break;
                    }
                }
            }
            closedir(proc_fd_s);
        }
    }

    closedir(dir_s);
}
int main(int argc, char *argv[])
{
    // 参数检查
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [filename].\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 遍历寻找符合要求的进程
    traverse_proc_pid(argv[1]);

    return 0;
}