/**
 * @file t_flock.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief flock 交互式加锁程序
 * @version 0.1
 * @date 2019-07-09
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd, lock;
    const char *lname;
    time_t timep;

    if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
            strchr("sx", argv[2][0]) == NULL) {
        fprintf(stderr, "Usage: %s file lock [sleep-time]\n"
                        "\t'lock' is 's' (shared) or 'x' (exclusive)\n"
                        "\t\toptionally followed by 'n' (nonblocking)\n"
                        "\t'secs' specifies time to hold lock\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
    if (argv[2][1] == 'n')
        lock |= LOCK_NB;

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

    time(&timep);
    printf("PID %ld: requesting %s at %s", (long) getpid(), lname, 
            ctime(&timep));

    if (flock(fd, lock) == -1) {
        if (errno == EWOULDBLOCK) {
            fprintf(stderr, "PID %ld: alread locked - bye!", (long) getpid());
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "flock (PID = %ld)", (long) getpid());
            exit(EXIT_FAILURE);
        }
    }

    time(&timep);
    printf("PID %ld: granted    %s at %s", (long) getpid(), lname,
            ctime(&timep));

    sleep((argc > 3) ? atoi(argv[3]) : 10);
    time(&timep);
    printf("PID %ld: releasing %s at %s", (long) getpid(), lname,
            ctime(&timep));
    if (flock(fd, LOCK_UN) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}