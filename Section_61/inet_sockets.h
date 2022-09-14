/**
 * @file inet_sockets.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief Internet domain socket库
 * @version 0.1
 * @date 2019-07-13
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _INET_SOCKETS_H_
#define _INET_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>

int inetConnect(const char *host, const char *service, int type);
int inetListen(const char *service, int backlog, socklen_t *addrlen);
int inetBind(const char *service, int type, socklen_t *addrlen);
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
                        char *addrStr, int addrStrLen);

#define  IS_ADDR_STR_LEN    4096
                            /* 传递给inetAddrStr 函数缓冲区长度建议值,
                             必须比(NI_MAXHOST + NI_MAXSERV + 4要大) */

#endif /* inet_sockets.h */