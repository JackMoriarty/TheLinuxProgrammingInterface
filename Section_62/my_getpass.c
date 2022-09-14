/**
 * @file my_getpass.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 实现 getpass 函数功能
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define CONTROL_TTY "/dev/tty"
#define TTY_CONTROL ECHO
#define PASSWD_LEN  256

static char passwd[PASSWD_LEN];

char *my_getpass(const char *prompt)
{
    int fd;
    int num_read;
    struct termios tp;

    /* 获取控制终端描述符 */
    fd = open(CONTROL_TTY, O_RDONLY);
    if (fd == -1)
        return NULL;

    /* 关闭终端回显 */
    if (tcgetattr(fd, &tp) == -1)
        return NULL;
    tp.c_lflag &= ~TTY_CONTROL;

    if (tcsetattr(fd, TCSAFLUSH, &tp) == -1)
        return NULL;

    /* 输出头部 */
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    num_read = read(STDIN_FILENO, passwd, PASSWD_LEN);
    if (num_read == -1)
        return NULL;
    passwd[num_read - 1] = '\0';
    putchar('\n');

    /* 打开终端回显 */
    if (tcgetattr(fd, &tp) == -1)
        return NULL;
    tp.c_lflag |= TTY_CONTROL;

    if (tcsetattr(fd, TCSAFLUSH, &tp) == -1)
        return NULL;
    
    close(fd);

    return passwd;
}

#ifdef TEST

#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *passwd;
    passwd = my_getpass("PASSWORD:");
    // passwd = getpass("passwd:");
    
    if (passwd != NULL)
        printf("%s\n", passwd);
    else 
        perror("my_getpass");
    
    exit(EXIT_SUCCESS);
}

#endif