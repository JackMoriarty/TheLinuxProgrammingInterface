/**
 * @file use_waitid.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 使用 waitid
 * @version 0.1
 * @date 2019-04-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void print_siginfo(siginfo_t *siginfo)
{
    printf("si_code: \t");
    switch (siginfo->si_code) {
    case CLD_EXITED:
        printf("CLD_EXITED\n");
        break;
    
    case CLD_KILLED:
        printf("CLD_KILLED\n");
        break;
    
    case CLD_STOPPED:
        printf("CLD_STOPPED\n");
        break;
    
    case CLD_CONTINUED:
        printf("CLD_CONTINUE\n");
        break;
    
    default:
        printf("Unexpect si_code\n");
        exit(EXIT_FAILURE);
    }

    printf("si_pid: \t%ld\n", (long)siginfo->si_pid);
    printf("si_signo:\t");
    switch (siginfo->si_signo) {
    case SIGCHLD:
        printf("SIGCHLD\n");
        break;
    
    default:
        printf("Unexpect si_signo\n");
        exit(EXIT_FAILURE);
    }

    printf("si_status: \t0x%04x\t%d\t%d\n",(unsigned int) siginfo->si_status, 
        siginfo->si_status >> 8, siginfo->si_status & 0xff);
    
    printf("si_uid: \t%ld\n", (long) siginfo->si_uid);

}

int main(int argc, char *argv[])
{
    int status;
    pid_t childPid;
    siginfo_t info;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [exit-status]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:

        printf("Child started with PID = %ld\n", (long)getpid());
        if (argc > 1) {
            status = atoi(argv[1]);
            _exit(status);
        } else {
            for (;;)
                pause();
        }
        _exit(EXIT_FAILURE);

    default:
        while(1) {
            childPid = waitid(P_ALL, 0, &info, WEXITED);
            if (childPid == -1) {
                perror("waitid");
                exit(EXIT_FAILURE);
            }

            printf("waitid() returned: PID = %ld;\n", (long) childPid);
            print_siginfo(&info);

            exit(EXIT_SUCCESS);
        }
    }
}