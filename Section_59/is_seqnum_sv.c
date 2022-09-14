/**
 * @file is_seqnum_sv.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用流socket与客户端进行通行的迭代式序号服务器
 * @version 0.1
 * @date 2019-07-12
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include "is_seqnum.h"

#define BACK_LOG    50

int main(int argc, char *argv[])
{
    uint32_t seqNum;
    char reqLenStr[INT_LEN];
    char seqNumStr[INT_LEN];
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct readline_buf rlbuf;
    
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s [init-seq-num]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seqNum = (argc > 1) ? atoi(argv[1]) : 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    /* 调用getaddrinfo获取尝试绑定的地址列表 */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    /* 允许使用IPv4或IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
                                    /* 不指定IP地址, 服务名为端口字符串 */
    
    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo");
        exit(EXIT_FAILURE);
    }

    /* 依次尝试各个地址, 直到找到符合要求的地址 */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;           /* 错误,尝试下一地址 */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
                == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;  /* success */
        
        /* failed */
        close(lfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "could not bind socket to any address\n");
        exit(EXIT_FAILURE);
    }

    if (listen(lfd, BACK_LOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    for (;;) {
        /* 迭代式处理请求 */
        /* 接收客户端连接, 获取客户端地址 */
        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen, 
                    host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        readline_buf_init(cfd, &rlbuf);
        if (readline_buf(&rlbuf, reqLenStr, INT_LEN) <= 0) {
            close (cfd);
            /* 读取请求失败, 跳过 */ 
            continue;
        }

        reqLen = atoi(reqLenStr);
        /* 请求信息有误 */
        if (reqLen <= 0) {
            close(cfd);
            continue;
        }

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
        if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
            fprintf(stderr, "Error on write\n");
        
        seqNum += reqLen;

        if (close(cfd) == -1) {
            fprintf(stderr, "close\n");
        }
    }

    exit(EXIT_SUCCESS);
}