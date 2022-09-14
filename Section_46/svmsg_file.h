/**
 * @file svmsg_file.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief svmsg_file_server.c 和 svmsg_file_client.c 公共头文件
 * @version 0.1
 * @date 2019-06-21
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

/* 服务器消息队列键值 */
#define SERVER_KEY 0x1aaaaaa1
/* 服务器消息队列标识符存储文件位置 */
#define SERVER_ID_FILE "/tmp/server_id"

struct requestMsg {             /* 请求信息 */
    long mtype;                 /* 未使用 */
    int clientId;               /* 客户端消息队列ID */
    char pathname[PATH_MAX];    /* 待返回的文件内容 */
};

/* 定义 clientId 和 pathname的大小*/
#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - \
                        offsetof(struct requestMsg, clientId) + PATH_MAX)

/* 相应部分消息大小 */
#define RESP_MSG_SIZE 8192

struct responseMsg {
    long mtype;
    char data[RESP_MSG_SIZE];
};

/* 响应信息类型 */
#define RESP_MT_FAILURE 1   /* 文件无法打开 */
#define RESP_MT_DATA    2   /* 文件数据 */
#define RESP_MT_END     3   /* 文件数据结束标志 */
