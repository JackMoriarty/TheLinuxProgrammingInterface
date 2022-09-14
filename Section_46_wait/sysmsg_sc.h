/**
 * @file sysmsg_sc.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 利用systemv消息队列实现数据传递公共头文件
 * @version 0.1
 * @date 2019-06-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _SYSMSG_SC_H_
#define _SYSMSG_SC_H_

#include <stddef.h>

/* 服务器端 System V 消息队列 key */
#define SERVER_KEY 0x1aaaaaa1

/* 请求信息格式 */
struct request_msg {
    long mtype;                     /* 消息类型, 未使用 */
    int client_id;                  /* 客户端消息队列ID */
    int seq_length;                 /* 客户端消息内容 */
};
/* 响应信息格式 */
struct response_msg {
    long mtype;                     /* 消息类型 */
    int seq_num;                    /* 服务端消息 */
};

/* 请求信息长度 */
#define REQ_MSG_SIZE (offsetof(struct request_msg, seq_length) -\
                        offsetof(struct request_msg, client_id) + sizeof(int))
/* 消息类型 */
#define RESP_MT_FAILURE 1   /* 服务器无法处理相关请求 */
#define RESP_MT_DATA    2   /* 包含数据的消息 */

#endif  /* ifndef _SYSMSG_SC_H_ */