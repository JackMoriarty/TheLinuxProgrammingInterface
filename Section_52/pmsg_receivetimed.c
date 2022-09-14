/**
 * @file pmsg_receivetimed.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 从POSIX消息队列中获取数据, 阻塞超时时不在继续等待
 * @version 0.1
 * @date 2019-07-07
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _POSIX_C_SOURCE  200112L

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

extern char *optarg;
extern int optind, optopt;

/**
 * @brief: 用法输出
 * 
 * @param progName: 程序名
*/
static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-t overtime] name\n", progName);
    fprintf(stderr, "\t -t overtime\t set overtime; default 10s\n");
    
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;
    time_t overtime = 10;
    struct timespec abs_overtime;

    flags = O_RDONLY;
    while((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
            case 't':
                overtime = atol(optarg);
                break;

            default: 
                usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    if (mq_getattr(mqd, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_REALTIME, &abs_overtime) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    abs_overtime.tv_sec += overtime;

    numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, &abs_overtime);
    if (numRead == -1) {
        perror("mq_receiebe");
        exit(EXIT_FAILURE);
    }

    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}