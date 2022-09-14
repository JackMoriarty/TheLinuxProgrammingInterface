#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>

#define CMD_SIZE 200

int main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t child_pid;

    setbuf(stdout, NULL);

    printf("Parent PID=%ld\n", (long)getpid());

    sigset_t block_mask, orig_mask;

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    switch (child_pid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        printf("Child (PID=%ld) exiting\n", (long)getpid());
        _exit(EXIT_SUCCESS);

    default:
        if (sigwaitinfo(&block_mask, NULL) == -1) {
            perror("sigwaitinfo");
            exit(EXIT_SUCCESS);
        }

        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        cmd[CMD_SIZE] = '\0';
        system(cmd);

        if (kill(child_pid, SIGKILL) == -1) {
            perror("kill");
            exit(EXIT_SUCCESS);
        }

        sleep(3);
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)child_pid);
        system(cmd);

        exit(EXIT_SUCCESS);
    }
}