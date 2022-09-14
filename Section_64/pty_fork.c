/**
 * @file pty_fork.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 将进程连接到伪终端
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "pty_master_open.h"
#include "pty_fork.h"

#define MAX_SNAME   1000

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
                const struct termios *slaveTermios, const struct winsize *slaveWs) {
    int mfd, slaveFd, savedErrno;
    pid_t childPid;
    char slname[MAX_SNAME];

    mfd = ptyMasterOpen(slname, MAX_SNAME);
    if (mfd == -1)
        return -1;
    
    if (slaveName != NULL) {
        if (strlen(slname) < snLen) {
            strncpy(slaveName, slname, snLen);
        } else {
            close(mfd);
            errno = EOVERFLOW;
            return -1;
        }
    }

    childPid = fork();
    if (childPid == -1) {
        savedErrno = errno;
        close(mfd);
        errno = savedErrno;
        return -1;
    }
    if (childPid != 0) {
        *masterFd = mfd;
        return childPid;
    }

    if (setsid() == -1) {
        perror("ptyFork:setsid");
        exit(EXIT_FAILURE);
    }

    close(mfd);

    slaveFd = open(slname, O_RDWR);
    if (slaveFd == -1) {
        perror("ptyFork:open-slave");
        exit(EXIT_FAILURE);
    }

#ifdef TIOCSCTTY
    if (ioctl(slaveFd, TIOCSCTTY, 0) == -1) {
        perror("ptyFork:ioctl-TIOCSCTTY");
        exit(EXIT_FAILURE);
    }
#endif

    if (slaveTermios != NULL)
        if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1) {
            perror("ptyFork:tcsetattr");
            exit(EXIT_FAILURE);
        }
    if (slaveWs != NULL)
        if (ioctl(slaveFd, TIOCSWINSZ, slaveWs) == -1) {
            perror("ptyFork:ioctl-TIOCSWINSZ");
            exit(EXIT_FAILURE);
        }

    if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO) {
        perror("ptyFork:dup2-STDIN_FILENO");
        exit(EXIT_FAILURE);
    }
    if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO) {
        perror("ptyFork:dup2-STDOUT_FILENO");
        exit(EXIT_FAILURE);
    }
    if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO) {
        perror("ptyFork:dup2-STDERR_FILENO");
        exit(EXIT_FAILURE);
    }

    if (slaveFd > STDERR_FILENO)
        close(slaveFd);
    
    return 0;
}