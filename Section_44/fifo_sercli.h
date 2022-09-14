#ifndef _FIFO_SERCLI_H_
#define _FIFO_SERCLI_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* 服务器FIFO位置 */
#define SERVER_FIFO "/tmp/seqnum_sv"
/* 客户端FIFO位置 */
#define CLIENT_FIFO_TEMPLATE    "/tmp/seqnum_cl.%ld"
/* 客户端FIFO路径长度, 增加20以为PID提供空间 */
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

/* 请求信息 */
struct request {
    pid_t pid;
    int seq_len;
};

/* 响应信息 */
struct response {
    int seq_num;
};

#endif