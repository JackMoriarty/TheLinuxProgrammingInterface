/**
 * @file my_execlp.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 利用 execv 实现 execlp
 * @version 0.1
 * @date 2019-04-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

extern char **environ;

#define max(x, y) ((x) > (y) ? (x) : (y))
#define SHELL_PATH "/bin/sh"

/**
 * @brief 执行脚本文件
 * 
 * @param argc 参数个数
 * @param argv 参数数组
 * @param envp 环境变量数组
 */
static void exec_sh_script(int argc, char *argv[], char *envp[])
{
    char **sh_argv;
    sh_argv = malloc(sizeof(void *) * (argc + 1));
    int j;

    sh_argv[0] = SHELL_PATH;
    for (j = 0; j <= argc; j++)
        sh_argv[j + 1] = argv[j];
    
    execve(SHELL_PATH, sh_argv, envp);
}

/**
 * @brief 自己实现的execlp
 * 
 * @param filename 文件名
 * @param arg 参数列表
 * @param ... 
 * @return int return -1 on error
 */
int my_execlp(const char *filename, const char *arg, ...)
{
    int argv_size;
    int argc;
    char **argv;
    char **envp;
    va_list arg_list;
    int j;
    char *pathname;
    int saved_errno;
    char *p;
    char *PATH;
    char *pr_start, *pr_end;
    int more_prefixes;
    int fnd_EACCES;

    fnd_EACCES = 0;

    /* 创建参数列表 */
    argv_size = 100;
    argv = calloc(argv_size, sizeof(void *));
    if (argv == NULL)
        return -1;
    
    argv[0] = (char *)arg;
    argc = 1;

    va_start(arg_list, arg);
    while (argv[argc - 1] != NULL) {
        if (argc + 1 >= argv_size) {
            char **nargv;

            argv_size += 100;
            nargv = realloc(argv, sizeof(void *) * argv_size);
            if (nargv == NULL) {
                free(argv);
                return -1;
            } else {
                argv = nargv;
            }
        }

        argv[argc] = va_arg(arg_list, char *);
        argc++;
    }
    va_end(arg_list);

    /* 使用调用者环境创建环境变量数组 */
    for (j = 0; environ[j] != NULL; j++);

    envp = calloc(j + 1, sizeof(void *));
    if (envp == NULL) {
        free(argv);
        return -1;
    }

    for (j = 0; environ[j] != NULL; j++)
        envp[j] = strdup(environ[j]);
    envp[j] = NULL;

    /* 尝试创建执行文件名 */

    if (strchr(filename, '/') != NULL) {
        /* 存在斜杠, 证明不需要使用PATH变量 */

        pathname = strdup(filename);
        execve(pathname, argv, envp);

        /* 执行失败, 说明可能是脚本文件 */
        saved_errno = errno;
        if (errno == ENOEXEC)
            exec_sh_script(argc, argv, envp);
        
        free(pathname);
    } else {
        /* 需要使用PATH环境变量 */
        p = getenv("PATH");
        PATH = (p == NULL || strlen(p) == 0) ? strdup(".") : strdup(p);

        pr_start = PATH;
        more_prefixes = 1;

        while (more_prefixes) {
            /* 获得前缀的末尾 */
            pr_end = strchr(pr_start, ':');
            if (pr_end == NULL)
                pr_end = pr_start + strlen(pr_start);

            /* 构建可执行文件的路径 */
            pathname = malloc(max(1, pr_end - pr_start) + strlen(filename) + 2);
            pathname[0] = '\0';
            if (pr_end == pr_start)
                strcat(pathname, ".");
            else 
                strncat(pathname, pr_start, pr_end - pr_start);
            strcat(pathname, "/");
            strcat(pathname, filename);

            if (*pr_end == '\0')
                more_prefixes = 0;
            else
                pr_start = pr_end + 1;

            /* 尝试执行 */
            execve(pathname, argv, envp);
            saved_errno = errno;
            if (errno == EACCES)
                fnd_EACCES = 1;
            else if (errno == ENOEXEC)
                exec_sh_script(argc, argv, envp);

            free(pathname);
        }

        free(PATH);
    }

    /* 到达此处时, 说明执行失败 */

    free(argv);
    for (j = 0; envp[j] != NULL; j++)
        free(envp[j]);
    free(envp);

    errno = fnd_EACCES ? EACCES : saved_errno;
    return -1;
}

#ifdef TEST

int main(int argc, char *argv[])
{
    my_execlp("echo", "echo", "hello", (char *)NULL);
    exit(EXIT_SUCCESS);
}

#endif