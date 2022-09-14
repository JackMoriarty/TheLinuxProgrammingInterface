/**
 * @file douser.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief sudo 类似程序
 * @version 0.1
 * @date 2019-05-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <limits.h>
#include <shadow.h>
#include <unistd.h>
#include <errno.h>

typedef enum {
    FALSE,
    TRUE
} boolean;

int main(int argc, char *argv[])
{
    char *username, *password, *encrypted, *p;
    struct passwd *pwd;
    struct spwd *spwd;
    boolean auth_ok;
    size_t len;
    long lnmax;

    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    /* 获取不到限制, 作假定处理 */
    if (lnmax == -1)
        lnmax = 256;

    username = (char *)malloc(lnmax * sizeof(char));
    if (username == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Username: ");
    fflush(stdout);
    if (fgets(username, lnmax * sizeof(char), stdin) == NULL) {
        /* 收到EOF时退出 */
        exit(EXIT_FAILURE);
    }

    len = strlen(username);
    /* 移除末尾的回车符 */
    if (username[len - 1] == '\n')
        username[len - 1] = '\0';

    pwd = getpwnam(username);
    if (pwd == NULL) {
        fprintf(stderr, "couldn't get password record\n");
        exit(EXIT_FAILURE);
    }
    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) {
        fprintf(stderr, "no permission to read shadow password file\n");
        exit(EXIT_FAILURE);
    }

    /* 存在shadow记录, 则使用shadow记录的密码 */
    if (spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;
    
    password = getpass("Password: ");

    /* 加密获得密码 */
    encrypted = crypt(password, pwd->pw_passwd);
    for (p = password; *p != '\0';)
        *(p++) = '\0';

    if (encrypted == NULL) {
        perror("crypt");
        exit(EXIT_FAILURE);
    }

    auth_ok = strcmp(encrypted, pwd->pw_passwd) == 0;
    if (!auth_ok) {
        printf("Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

    if (setuid(pwd->pw_uid) == -1) {
        perror("setuid");
        exit(EXIT_FAILURE);
    }
    if (setgid(pwd->pw_gid) == -1) {
        perror("setgid");
        exit(EXIT_FAILURE);
    }

    execvp(argv[1], &argv[1]);

    exit(EXIT_SUCCESS);
}