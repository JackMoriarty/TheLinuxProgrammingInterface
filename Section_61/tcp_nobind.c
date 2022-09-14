/**
 * @file tcp_nobind.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief tcp未绑定直接调用监听, 内核会分配临时端口验证
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_storage ss;
    int ss_size;
    char host_name[NI_MAXHOST];
    char port_name[NI_MAXSERV];

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    listen(sfd, 10);

    ss_size = sizeof(struct sockaddr_storage);
    getsockname(sfd, (struct sockaddr *)&ss, (socklen_t *)&ss_size);

    getnameinfo((struct sockaddr *)&ss, ss_size, host_name, NI_MAXHOST,
                    port_name, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    printf("[%s:%s]\n", host_name, port_name);
    
    close(sfd);

    exit(EXIT_SUCCESS);
}