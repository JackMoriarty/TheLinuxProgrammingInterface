/**
 * @file sysvmsgclient.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 利用systemv消息队列实现数据传递客户端
 * @version 0.1
 * @date 2019-06-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "sysmsg_sc.h"

static int client_id;

static void remove_queue(void);

/**
 * @brief SIGALRM 信号处理函数
 * 
*/
static void sig_alarm(int sig)
{
    remove_queue();
    _exit(EXIT_FAILURE);
}

/**
 * @brief 删除客户端消息队列
 * 
*/
static void remove_queue(void)
{
    if (msgctl(client_id, IPC_RMID, NULL) == -1) {
        perror("client msgctl");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    struct request_msg req;
    struct response_msg resp;
    int server_id;
    int msg_len;
    struct sigaction sa;
    int savedErrno;

    /* 获取服务器消息队列标识符, 创建客户端消息队列 */
    server_id = msgget(SERVER_KEY, S_IWUSR);
    if (server_id == -1) {
        perror("msgget - server message queue");
        exit(EXIT_FAILURE);
    }

    client_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                        S_IRUSR | S_IWUSR | S_IWGRP);
    if (client_id == -1) {
        perror("msgget - client message queue");
        exit(EXIT_FAILURE);
    }

    /* 建立SIG_ALRM信号处理函数 */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = griimReaper;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("server sigaction");
        exit(EXIT_FAILURE);
    }

    if (atexit(remove_queue) != 0) {
        perror("client atexit");
        exit(EXIT_FAILURE);
    }

    req.mtype = 1;
    req.client_id = client_id;
    req.seq_length = 1;
    alarm(10);
    if (msgsnd(server_id, &req, REQ_MSG_SIZE, 0) == -1) {
        perror("client msgsnd");
        exit(EXIT_FAILURE);
    }
    savedErrno = errno;
    alarm(0);
    errno = savedErrno;

    alarm(10);
    msg_len = msgrcv(client_id, &resp, sizeof(int), 0, 0);
    if (msg_len == -1) {
        perror("client msgrcv");
        exit(EXIT_FAILURE);
    }
    savedErrno = errno;
    alarm(0);
    errno = savedErrno;

    if (resp.mtype == RESP_MT_FAILURE) {
        fprintf(stderr, "server failed\n");
        exit(EXIT_FAILURE);
    } else {
        printf("%d\n", resp.seq_num);
    }

    exit(EXIT_SUCCESS);
}