/**
 * @file my_pipe.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 自己实现的 pipe 函数
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <sys/socket.h>
#include "my_pipe.h"

int my_pipe(int fd[2])
{
    int retval;
    retval = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    /* 错误只可能是EMFILE 和 ENFILE */
    if (retval != 0)
        return -1;
    
    /* 应该不会出错 */
    shutdown(fd[0], SHUT_WR);
    shutdown(fd[1], SHUT_RD);

    return 0;
}