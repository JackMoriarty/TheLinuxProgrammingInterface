/**
 * @file sig_speed_sigsuspend.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309
#include <signal.h>

static void
handler(int sig)
{
}

#define TESTSIG SIGUSR1

int
main(int argc, char *argv[])
{
    int numSigs, scnt;
    pid_t childPid;
    sigset_t blockedMask, emptyMask;
    struct sigaction sa;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s num-sigs\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    numSigs = strtol(argv[1], NULL, 0);

    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sa.sa_handler = handler;
    // if (sigaction(TESTSIG, &sa, NULL) == -1) {
    //     perror("sigaction");
    //     exit(EXIT_FAILURE);
    // }

    /* Block the signal before fork(), so that the child doesn't manage
       to send it to the parent before the parent is ready to catch it */

    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&emptyMask);
    sigaddset(&emptyMask, TESTSIG);

    switch (childPid = fork()) {
    case -1: perror("fork");
        exit(EXIT_FAILURE);

    case 0:     /* child */
        for (scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            
            if (sigwaitinfo(&emptyMask, NULL) == -1 && errno != EINTR) {
            // if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                exit(EXIT_FAILURE);
            }
                
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (scnt = 0; scnt < numSigs; scnt++) {
            if (sigwaitinfo(&emptyMask, NULL) == -1 && errno != EINTR) {
            // if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                exit(EXIT_FAILURE);
            }
                
            if (kill(childPid, TESTSIG) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
}

