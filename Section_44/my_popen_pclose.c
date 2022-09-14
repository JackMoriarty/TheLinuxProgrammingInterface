/**
 * @file my_popen_pclose.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现`popen`和`pclose`
 * @version 0.1
 * @date 2019-06-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/wait.h>

static pid_t *fd_pid = NULL;
static int fd_pid_sum = 0;

/**
 * @brief 打开管道并执行shell程序
 * 
 * @param command `shell`命令
 * @param mode 打开模式`w`或`r`
 * @return FILE* 文件流指针, 错误时返回NULL
 */
FILE *my_popen(const char *command, const char *mode)
{
    int fd[2];
    int is_pw; /* 调用进程是否为写端 */
    pid_t child_pid;
    FILE *fp;
    int open_file_max;
    int parent_pipe_fd;

    /* 传参检查 */
    if (strcmp(mode, "w") && strcmp(mode, "r")) {
        errno = EINVAL;
        return NULL;
    }
    is_pw = strcmp(mode, "r");

    /* 打开管道 */
    if (pipe(fd) == -1) {
        return NULL;
    }

    /* 创建子进程 */
    switch ((child_pid = fork())) {
        case -1: /* 创建子进程失败 */
            /* 关闭管道, 忽略错误 */
            close(fd[0]);
            close(fd[1]);
            return NULL;

        case 0: /* 子进程 */
            if (is_pw) {
                /* 关闭子进程写端 */
                if (close(fd[1]) == -1) {
                    _exit(EXIT_FAILURE);
                }
                /* 将子进程的标准输入设置为管道读端 */
                if (dup2(fd[0] ,STDIN_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                /* 关闭子进程读端*/
                if (close(fd[0]) == -1) {
                    _exit(EXIT_FAILURE);
                }
            } else {
                /* 关闭子进程读端 */
                if (close(fd[0]) == -1) {
                    _exit(EXIT_FAILURE);
                }
                /* 将子进程的标准输出和错误输出设置为管道写端 */
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                if (dup2(fd[1], STDERR_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                /* 关闭子进程写端 */
                if (close(fd[1]) == -1) {
                    _exit(EXIT_FAILURE);
                }
            }

            /* 执行相关命令 */
            execl("/bin/sh", "sh", "-c", command, (char *)NULL);

            /* 应该执行不到这里 */
            _exit(EXIT_FAILURE);
            break;

        default: /* 父进程 */
            /* 创建登记表 */
            if (fd_pid_sum == 0) {
                open_file_max = sysconf(_SC_OPEN_MAX);
                fd_pid = (pid_t *)malloc(sizeof(pid_t) * open_file_max);
                if (fd_pid == NULL) {
                    return NULL;
                }
            }

            fd_pid_sum++;

            if (is_pw) {
                /* 关闭父进程读端 */
                if (close(fd[0]) == -1) {
                    close(fd[1]);
                    return NULL;
                }
                parent_pipe_fd = fd[1];
            } else {
                /* 关闭父进程写端 */
                if (close(fd[1]) == -1) {
                    close(fd[0]);
                    return NULL;
                }
                parent_pipe_fd = fd[0];
            }

            /* 打开文件流 */
            fp = fdopen(parent_pipe_fd, mode);
            if (fp == NULL) {
                /* 关闭管道 */
                close(parent_pipe_fd);
                return NULL;
            }
            /* 登记文件流 */
            fd_pid[parent_pipe_fd] = child_pid;
            return fp;
    }

}

/**
 * @brief 关闭管道
 * 
 * @param stream 文件流
 * @return int 子进程返回状态, -1表示错误
 */
int my_pclose(FILE *stream)
{
    pid_t child_pid;
    int fd;
    int retval;

    /* 查找登记表 */
    fd = fileno(stream);
    child_pid = fd_pid[fd];

    /* 释放登记表 */
    fd_pid_sum--;
    if (fd_pid_sum == 0) {
        free(fd_pid);
        fd_pid = NULL;
    }

    /* 关闭文件流 */
    if (fclose(stream) == EOF) {
        return -1;
    }
    /* 等待子进程 */
    if (waitpid(child_pid, &retval, WUNTRACED) == -1) {
        return -1;
    }

    return retval;
}

int main(int argc, char *argv[])
{
    FILE *fp1, *fp2;
    char buffer[256];
    fp1 = my_popen("echo hello", "r");
    fscanf(fp1, "%s", buffer);
    puts(buffer);
    fp2 = my_popen("cat", "w");
    fprintf(fp2, "world\n");
    my_pclose(fp1);
    my_pclose(fp2);
    exit(EXIT_FAILURE);
}