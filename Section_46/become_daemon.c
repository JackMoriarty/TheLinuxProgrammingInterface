#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "become_daemon.h"

/**
 * @brief 创建的daemon进程
 * 
 * @param flags 标志位
 * @return int 0 表示成功, -1 表示失败
*/
int become_daemon(int flags)
{
    int maxfd, fd;

    /* 成为后台函数 */
    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    /* 成为新会话首进程 */
    if (setsid() == -1)
        return -1;
    /* 确保本进程不是会话首进程 */
    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    /* 清除进程umask掩码 */
    if (!(flags & BD_NO_UMASKD))
        umask(0);

    /* 改变工作目录为根目录 */
    if (!(flags & BD_NO_UMASKD))
        chdir("/");

    /* 关闭所有已打开的文件描述符 */
    if (!(flags & BD_NO_CLOSE_FILES)) {
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)
            maxfd = BD_MAX_CLOSE;

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    /* 重新打开标准输入输出到 /dev/null */
    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);

        /* fd 应当为0 */
        if (fd != STDIN_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}