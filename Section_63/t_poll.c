/**
 * @file t_poll.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用poll来检查多个文件描述符
 * @version 0.1
 * @date 2019-07-18
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s {timeout|-} fd-num[rw]...\n", progName);
    fprintf(stderr, "   - means infinite timeout;\n");
    fprintf(stderr, "   r = monitor for read\n");
    fprintf(stderr, "   w = monitor for write\n\n");
    fprintf(stderr, "   e.g.: %s - 0rw 1w\n", progName);
    
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int pto, nfds, i, j, numRead, fd, ready;
    struct pollfd *fds;
    char buf[10];

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageError(argv[0]);
    
    if (strcmp(argv[1], "-") == 0)
        pto = -1;
    else
        pto = atoi(argv[1]) * 1000;

    nfds = argc - 2;
    
    fds = calloc(nfds, sizeof(struct pollfd));
    if (fds == NULL) {
        perror("calloc for fds");
        exit(EXIT_FAILURE);
    }

    for (i = 2, j = 0; i < argc; i++, j++) {
        numRead = sscanf(argv[i], "%d%2[rw]", &fd, buf);
        if (numRead != 2) {
            free(fds);
            usageError(argv[0]);
        }

        fds[j].fd = fd;
        if (strchr(buf, 'r') != NULL)
            fds[j].events |= POLLIN;
        if (strchr(buf, 'w') != NULL)
            fds[j].events |= POLLOUT;
    }

    ready = poll(fds, nfds, pto);
    if (ready == -1) {
        perror("poll");
        free(fds);
        exit(EXIT_FAILURE);
    }
    
    printf("ready = %d\n", ready);
    for (j = 0; j < nfds; j++) {
        if (fds[j].revents &(POLLIN | POLLOUT))
            printf("%d: %s%s\n", fds[j].fd, 
                    (fds[j].revents & POLLIN) ? "r" : "",
                    (fds[j].revents & POLLOUT) ? "w" : "");
    }

    exit(EXIT_SUCCESS);
}