/**
 * @file sysvmsgserver.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 利用sysvmsg消息队列实现数据传递服务器端
 * @version 0.1
 * @date 2019-06-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include "sysmsg_sc.h"

/**
 * @brief 处理终止的子进程, SIGCHLD信号处理函数
 * 
 * @param sig 信号值
*/
static void grim_reaper(int sig)
{
    int save_errno;

    save_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = save_errno;
}

/**
 * @brief 服务器响应函数
 * 
 * @param client_id 对应客户端消息队列标识符
 * @param mtype 相应信息类型
 * @param seq_num 相应数据
*/
static void server_request(int client_id, int mtype, int seq_num)
{
    struct response_msg rmsg;

    rmsg.mtype = mtype;
    rmsg.seq_num = seq_num;
    if (msgsnd(client_id, &rmsg, sizeof(int), 0) == -1)
        fprintf(stderr, "Failed to send msg to chient_id: %d\n", client_id);
}

int main(int argc, char *argv[])
{
    struct request_msg req;
    struct sigaction sa;
    int server_id;
    int msg_len;
    int seq_num = 0;
    int client_seq_num;
    pid_t pid;
    int mtype;

    /* 创建服务器消息队列 */
    server_id = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL |
                        S_IRUSR | S_IWUSR | S_IWGRP);
    if (server_id == -1) {
        perror("server msgget");
        exit(EXIT_FAILURE);
    }

    /* 建立 SIGCHLD 信号处理 */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("server sigaction");
        exit(EXIT_FAILURE);
    }

    /* 读取请求并响应 */
    for (;;) {
        msg_len = msgrcv(server_id, &req, REQ_MSG_SIZE, 0, 0);
        if (msg_len == -1) {
            /* 信号处理打断系统调用 */
            if (errno == EINTR)
                continue;
            /* 其他错误 */
            perror("server msgrcv");
            break;
        }

        client_seq_num = seq_num;
        if (seq_num + req.seq_length < INT_MAX) {
            seq_num += req.seq_length;
            mtype = RESP_MT_DATA;
        } else {
            seq_num = 0;
            mtype = RESP_MT_FAILURE;
        }
        

        pid = fork();
        if (pid == -1) {
            perror("server fork");
            break;
        }
        if (pid == 0) {
            /* 子进程 */
            server_request(req.client_id, mtype, client_seq_num);
            _exit(EXIT_SUCCESS);
        }
        
        /* 父进程继续循环处理请求 */
    }

    /* 删除服务器消息队列 */
    if (msgctl(server_id, IPC_RMID, NULL) == -1) {
        perror("server msgctl");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}