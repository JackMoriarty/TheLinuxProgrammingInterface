/**
 * @file script.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief script 命令实现
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "pty_fork.h"
#include "tty_functions.h"

#define BUF_SIZE    256
#define MAX_SNAME   1000

struct termios ttyOrig;
volatile sig_atomic_t gotSig = 0;
struct winsize ws;
int masterFd;

static void ttyReset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

static void sigwinchHandler(int sig)
{
    gotSig = 1;
}

void change_windows()
{
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl-STDIN_FILENO-TIOCGWINSZ");
        exit(EXIT_FAILURE);
    }
    if (ioctl(masterFd, TIOCSWINSZ, &ws) == -1) {
        perror("ioctl-masterFd-TIOCSWINSZ");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    char slaveName[MAX_SNAME];
    char *shell;
    int scriptFd, script_time_fd;
    fd_set inFds;
    char buf[BUF_SIZE];
    char timedbuf[BUF_SIZE];
    ssize_t numRead;
    pid_t childPid, childPid2;
    struct sigaction sa;
    time_t timep;

    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
        perror("ioctl-TIOCWINSZ");
        exit(EXIT_FAILURE);
    }

    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
    if (childPid == -1) {
        perror("ptyFork");
        exit(EXIT_FAILURE);
    }

    if (childPid == 0) {
        shell = getenv("SHELL");
        if (shell == NULL || *shell == '\0')
            shell = "/bin/sh";
        
        execlp(shell, shell, (char *) NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    scriptFd = open((argc > 1) ? argv[1] : "typescript",
                    O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                    S_IROTH | S_IWOTH);
    if (scriptFd == -1) {
        perror("open typescript");
        exit(EXIT_FAILURE);
    }
    if (argc > 1) {
        strncpy(buf, argv[1], BUF_SIZE - 1);
        strncat(buf, ".timed", BUF_SIZE - strlen(buf) - 1);
    }

    script_time_fd = open((argc > 1) ? buf : "typescript.timed",
                    O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                    S_IROTH | S_IWOTH);
    if (script_time_fd == -1) {
        perror("open script_time");
        exit(EXIT_FAILURE);
    }

    ttySetRaw(STDIN_FILENO, &ttyOrig);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    // sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigwinchHandler;
    if (sigaction(SIGWINCH, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (atexit(ttyReset) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    strncpy(buf, "Script start-->\n", BUF_SIZE);
    write(scriptFd, buf, strlen(buf));
    
    for (;;) {
        FD_ZERO(&inFds);
        FD_SET(STDIN_FILENO, &inFds);
        FD_SET(masterFd, &inFds);

        if (gotSig == 1)
            change_windows();

        if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1) {
            if (errno == EINTR)
                continue;
    
            perror("SELECT");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &inFds)) {
            numRead = read(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(masterFd, buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (masterFd)\n");
        }

        if (FD_ISSET(masterFd, &inFds)) {
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0) {
                strncpy(buf, "<--Script end\n", BUF_SIZE);
                write(scriptFd, buf, strlen(buf));
                exit(EXIT_SUCCESS);
            }
            
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (STDOUT_FILE)");
            if (write(scriptFd, buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (scriptFd)");
            
            if (time(&timep) == -1) {
                perror("get time");
                exit(EXIT_FAILURE);
            }
            strcpy(timedbuf, ctime(&timep));
            timedbuf[strlen(timedbuf) - 1] = ' ';
            if (write(script_time_fd, timedbuf, strlen(timedbuf) - 1) != 
                strlen(timedbuf) - 1) {
                fprintf(stderr, "partial/failed write (script_timed_fd)");
            }
            if (write(script_time_fd, buf, numRead) != numRead)
                fprintf(stderr, "partial/failed write (scriptFd)");
            timedbuf[0] = '\n';
            if (write(script_time_fd, timedbuf, 1) != 1)
                fprintf(stderr, "partial/failed write (scriptFd)");
        }
    }

    // childPid2 = fork();
    // if (childPid2 == -1) {
    //     perror("fork2");
    //     exit(EXIT_FAILURE);
    // }

    // if (childPid2 == 0) {
    //     for (;;) {
    //         numRead = read(STDIN_FILENO, buf, BUF_SIZE);
    //         if (numRead == -1) {
    //             if (errno == EINTR)
    //                 continue;

    //             perror("childPid2 read");
    //             exit(EXIT_FAILURE);
    //         } else if (numRead == 0) {
    //             exit(EXIT_SUCCESS);
    //         }
            
    //         if (write(masterFd, buf, numRead) != numRead) {
    //             fprintf(stderr, "patrial/failed write (masterFd)\n");
    //             exit(EXIT_FAILURE);
    //         }

    //         buf[numRead] = '\n';
    //         if (strchr(buf, '\4') != NULL) {
    //             exit(EXIT_SUCCESS);
    //         }
    //     }
    // } else {
    //     for (;;) {
    //         if (gotSig == 1)
    //             change_windows();
            
    //         numRead = read(masterFd, buf, BUF_SIZE);
    //         if (numRead == -1) {
    //             if (errno == EINTR)
    //                 continue;
    //             perror("parent read");
    //             exit(EXIT_FAILURE);
    //         } else if (numRead == 0) {
    //             strncpy(buf, "<--Script end\n", BUF_SIZE);
    //             write(scriptFd, buf, strlen(buf));
    //             exit(EXIT_SUCCESS);
    //         }
            
    //         if (write(STDOUT_FILENO, buf, numRead) != numRead)
    //             fprintf(stderr, "partial/failed write (STDOUT_FILE)");
    //         if (write(scriptFd, buf, numRead) != numRead)
    //             fprintf(stderr, "partial/failed write (scriptFd)");
    //     }
    // }
}