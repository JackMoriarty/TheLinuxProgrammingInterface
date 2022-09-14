/**
 * @file is_seqnum_cl.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 
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
#include "is_seqnum.h"

int main(int argc, char *argv[])
{
    char *reqLenStr;
    char seqNumStr[INT_LEN];
    int cfd;
    ssize_t numRead;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct readline_buf rlbf;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s server-host [sequence-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;
        
        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(cfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not connect socket to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    reqLenStr = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqLenStr, strlen(reqLenStr)) != strlen(reqLenStr)) {
        fprintf(stderr, "Partial/failed write (reqLenStr)\n");
        exit(EXIT_FAILURE);
    }
    if (write(cfd, "\n", 1) != 1) {
        fprintf(stderr, "Partial/failed write (new line)\n");
        exit(EXIT_FAILURE);
    }

    readline_buf_init(cfd, &rlbf);
    numRead = readline_buf(&rlbf, seqNumStr, INT_LEN);
    if (numRead == -1) {
        perror("readline_buf");
        exit(EXIT_FAILURE);
    }
    if (numRead == 0) {
        fprintf(stderr, "Unexpect EOF from server\n");
        exit(EXIT_FAILURE);
    }

    printf("Sequence number: %s", seqNumStr);

    exit(EXIT_SUCCESS);

}