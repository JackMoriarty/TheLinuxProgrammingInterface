#include <syslog.h>
#include "svmsg_file.h"
#include "become_daemon.h"

static int serverId;

void rm_id_file(void);

/* SIGCHLD 处置函数 */
static void griimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) >0)
        continue;
    errno = savedErrno;
}

/* SIGINT 和 SIGTERM 处置函数 */
static void sig_int_term(int sig)
{
    /* 移除消息队列和相应文件 */
    msgctl(serverId, IPC_RMID, NULL);
    rm_id_file();

    /* 恢复默认处置并重新发送此信号给自己 */
    signal(sig, SIG_DFL);
    raise(sig);
}

/* 定时器信号处置函数 */
static void sig_alarm_handler(int sig)
{
    /* 移除消息队列和相应文件 */
    syslog(LOG_ALERT, "server send over time\n");
    rm_id_file();
}

/* 子进程执行: 服务单个客户端 */
static void serverRequest(const struct requestMsg *req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;
    int savedErrno;

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {
        resp.mtype = RESP_MT_FAILURE;
        syslog(LOG_WARNING, "file %s can't be open: %m", req->pathname);
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        _exit(EXIT_FAILURE);
    }

    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        alarm(10);
        if (msgsnd(req->clientId, &resp, numRead, 0) == -1) {
            syslog(LOG_ERR, "read failed: %m");
            break;
        }
        savedErrno = errno;
        alarm(0);
        errno = savedErrno;
    }

    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0);
}

/* 删除服务器消息队列id文件 */
void rm_id_file(void)
{
    unlink(SERVER_ID_FILE);
    closelog();
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    struct sigaction sa;
    int fd;

    /* 成为daemon进程 */
    become_daemon(0);

    /* 创建服务器消息队列 */
    // serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL |
    //                     S_IRUSR | S_IWUSR |S_IWGRP);
    serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                        S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1) {
        perror("server msgget");
        exit(EXIT_FAILURE);
    }
    /* 打开并写入文件 */
    fd = open(SERVER_ID_FILE, O_CREAT | O_EXCL | O_SYNC | O_WRONLY,
                    S_IWUSR | S_IRUSR);
    if (fd == -1) {
        perror("server open failed");
        exit(EXIT_FAILURE);
    }

    /* 注册清除文件函数 */
    if (atexit(rm_id_file) != 0) {
        perror("server atexit");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &serverId, sizeof(int)) != sizeof(int)) {
        perror("server write");
        exit(EXIT_FAILURE);
    }
    close(fd);


    /* 建立SIG_CHLD信号处理函数 */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = griimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("server sigaction");
        exit(EXIT_FAILURE);
    }
    /* 建立SIG_INT和SIG_TERM信号处理函数 */
    sa.sa_flags = 0;
    sa.sa_handler = sig_int_term;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("server sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("server sigaction");
        exit(EXIT_FAILURE);
    }

    /* 读取请求, 并创建子进程进行处理 */
    for (;;) {
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            /* 被 SIGCHLD 信号处理器中断 */
            if (errno == EINTR)
                continue;
            fprintf(stderr, "msgrcv\n");
            break;
        }

        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "fork");
            break;
        }
        if (pid == 0) {
            serverRequest(&req);
            _exit(EXIT_SUCCESS);
        }
        /* 父进程循环处理下一个请求 */
    }

    /* 出错, 移除服务器消息队列 */
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        perror("server msgctl");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}