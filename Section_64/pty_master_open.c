/**
 * @file pty_master_open.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 打开主设备
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _XOPEN_SOURCE   600

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "pty_master_open.h"

int ptyMasterOpen(char *slaveName, size_t snLen)
{
    int masterFd, savedErrno;
    char *p;

    masterFd = posix_openpt(O_RDWR | O_NOCTTY);
    if (masterFd == -1)
        return -1;

    if (grantpt(masterFd) == -1) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;
        return -1;
    }

    if (unlockpt(masterFd) == -1) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;
        return -1;
    }

    p = ptsname(masterFd);
    if (p == NULL) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;
        return -1;
    }

    if (strlen(p) < snLen) {
        strncpy(slaveName, p, snLen);
    } else {
        close(masterFd);
        errno = EOVERFLOW;
        return -1;
    }

    return masterFd;
}