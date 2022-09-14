/**
 * @file pty_fork.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 将进程连接到伪终端
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _PTY_FORK_H_
#define _PTY_FORK_H_

#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
                const struct termios *slaveTermios, const struct winsize *slaveWs);

#endif /* pty_fork.h */