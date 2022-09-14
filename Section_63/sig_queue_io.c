/**
 * @file sig_queue_io.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用实时信号进行信号驱动IO
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define SIG_NO  SIGRTMIN

static volatile sig_atomic_t gotSig = 0;

static void sig_handler(int sig, siginfo_t *siginfo, void *ucontext)
{
    gotSig = 1;
    
    printf("fd: %ld\n", (long)siginfo->si_fd);
    printf("-->%s%s%s%s%s%s\n", 
            (siginfo->si_code & POLL_IN) ? "POLL_IN" : "-",
            (siginfo->si_code & POLL_OUT) ? "POLL_OUT" : "-",
            (siginfo->si_code & POLL_MSG) ? "POLL_MSG" : "-",
            (siginfo->si_code & POLL_ERR) ? "POLLERR" : "-",
            (siginfo->si_code & POLL_PRI) ? "POLL_PRI" : "-",
            (siginfo->si_code & POLL_HUP) ? "POLL_HUP" : "-");
}

static int tty_set_cbreak(int fd, struct termios *prevTermios)
{
    struct termios t;

    if (tcgetattr(fd, &t) == -1)
        return -1;
    if (prevTermios != NULL)
        *prevTermios = t;

    t.c_lflag &= ~(ICANON | ECHO);
    t.c_lflag |= ISIG;

    t.c_iflag &= ~ICRNL;
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
        return -1;
    
    return 0;
}

int main(int argc, char *argv[])
{
    int flags, j, cnt;
    struct termios origTermios;
    char ch;
    struct sigaction sa;
    int done;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sig_handler;
    if (sigaction(SIG_NO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1) {
        perror("fcntl-SETOWN");
        exit(EXIT_FAILURE);
    }

    if (fcntl(STDIN_FILENO, F_SETSIG, SIG_NO) == -1) {
        perror("fcntl-SETSIG");
        exit(EXIT_FAILURE);
    }

    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (tty_set_cbreak(STDIN_FILENO, &origTermios) == -1) {
        perror("tt_set_cbreak");
        exit(EXIT_FAILURE);
    }

    for (done = 0, cnt = 0; !done; cnt++) {
        for (j = 0; j < 100000000; j++)
            continue;
        
        if (gotSig){
            while (read(STDIN_FILENO, &ch, 1) > 0 && !done) {
                printf("cnt=%d; read %c\n", cnt, ch);
                done = ch == '#';
            }

            gotSig = 0;
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}