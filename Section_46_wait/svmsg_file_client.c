#include "svmsg_file.h"

static int clientId;

static void removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        perror("client msgctl");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int serverId, numMsgs;
    ssize_t msgLen, totBytes;
    int fd;
    int read_num;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
        fprintf(stderr, "pathname too long max: %ld bytes\n",
                (long)sizeof(req.pathname) - 1);
        exit(EXIT_FAILURE);
    }

    /* 获取服务器消息队列描述符, 创建客户端消息队列 */
    // serverId = msgget(SERVER_KEY, S_IWUSR);
    // if (serverId == -1) {
    //     perror("client msgget - server message queue");
    //     exit(EXIT_FAILURE);
    // }
    fd = open(SERVER_ID_FILE, O_RDONLY);
    if (fd == -1) {
        perror("client open server_id_file failed");
        exit(EXIT_FAILURE);
    }
    read_num = read(fd, &serverId, sizeof(int));
    if (read_num != sizeof(int)) {
        perror("client read from server_id_file failed");
        exit(EXIT_FAILURE);
    }
    close(fd);

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) {
        fprintf(stderr, "client msgget - client message queue\n");
        exit(EXIT_FAILURE);
    }

    if (atexit(removeQueue) != 0) {
        perror("client atexit");
        exit(EXIT_FAILURE);
    }

    /* 发送请求 */
    req.mtype = 1;  /* 类型不重要 */
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';  /* 保证字符串是有结尾的 */

    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) {
        perror("client msgsnd");
        exit(EXIT_FAILURE);
    }

    /* 获取响应 */
    msgLen = msgrcv(clientId, &req, REQ_MSG_SIZE, 0, 0);
    if (msgLen == -1) {
        perror("client msgrcv");
        exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);
        if (msgctl(clientId, IPC_RMID, NULL) == -1) {
            perror("client msgctl");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    /* 文件成功打开, 服务器返回文件中的数据 */
    totBytes = msgLen;
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            perror("client msgrcv");
            exit(EXIT_FAILURE);
        }

        totBytes += msgLen;
    }

    printf("Recieved %ld bytes (%d messages)\n", (long)totBytes, numMsgs);
    
    exit(EXIT_SUCCESS);
}