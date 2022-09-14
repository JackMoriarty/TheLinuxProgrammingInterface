/**
 * @file user_process.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 获取指定用户的所有进程信息
 * @version 0.1
 * @date 2019-01-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>

#define BUFFER_SIZE 1024

/**
 * @brief 获取指定用户的uid
 * 
 * @param name 用户名
 * @return uid_t 指定用户的UID
 */
uid_t get_user_uid(const char *name)
{
    struct passwd *user_pw;

    if (name == NULL || *name == '\0') {
        return -1;
    }
    user_pw = getpwnam(name);
    if (user_pw == NULL) {
        return -1;
    }

    return user_pw->pw_uid;
}

/**
 * @brief 打印相关文件信息
 * 
 * @param fp 文件指针
 * @param user_id 指定用户ID
 */
void print_file_message(FILE * fp, uid_t user_id)
{
    char buffer[BUFFER_SIZE];
    char program_name[NAME_MAX];
    pid_t program_pid;
    uid_t local_uid;
    char *name = NULL;
    char *value = NULL;

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        // 除去buffer中末尾的换行符
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        
        // 查找所需项目的行数
        name = strtok(buffer, ":");

        if (name != NULL && strcmp(name, "Uid") == 0) {
            // 判断是否是要求用户的进程

            while ((value = strtok(NULL, "\t")) != NULL && *value == '\0');
            if (value != NULL) {
                local_uid = strtol(value, NULL, 10);
                if (local_uid != user_id) {
                    // 不是要找的目标用户
                    return ;
                }
            }
        } else if (name != NULL && strcmp(name, "Name") == 0) {
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

    printf("%-50s\t:%ld\n", program_name, (long)program_pid);
}

/**
 * @brief 遍历`/proc/PID`文件夹
 * 
 * @param user_uid 用户uid
 */
void search_and_print(const uid_t user_uid)
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
                print_file_message(fp, user_uid);
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

int main(int argc, char *argv[])
{
    uid_t user_real_uid;

    // 参数检查
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [UserName].\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 获取指定用户的用户ID(实际ID)
    user_real_uid = get_user_uid(argv[1]);
    if (user_real_uid == -1) {
        fprintf(stderr, "Can not found %s or something error!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // 遍历`/proc/PID`目录, 并输出属于指定用户的进程相关信息
    search_and_print(user_real_uid);

    return 0;
}