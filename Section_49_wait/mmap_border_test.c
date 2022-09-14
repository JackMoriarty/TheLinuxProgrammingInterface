/**
 * @file mmap_border_test.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief mmap 文件映射边界情况
 * @version 0.1
 * @date 2019-07-06
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @brief 信号处置函数
 * 
 * @param sig: 信号值
*/
void sig_handler(int sig)
{
    // UNSAFE: printf 函数不是异步信号安全函数
    if (sig == SIGBUS) {
        printf("receieve SIGBUS\n");
    } else {
        printf("receieve SIGSEGV\n");
    }
}
int main(int argc, char *argv[])
{
    int fd;
    off_t file_offset;
    off_t file_len;
    void *mmap_addr;
    struct sigaction sigact;
    struct stat statbuf;
    int read_msg;

    /* 参数检查并解析 */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename offset\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    file_offset = atol(argv[2]);


    /* 打开指定文件并获取文件信息 */
    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open file");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &statbuf) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    file_len = statbuf.st_size;


    /* 挂载信号处置函数 */
    sigact.sa_handler = sig_handler;
    if (sigemptyset(&(sigact.sa_mask)) == -1) {
        perror("sigempty");
        exit(EXIT_FAILURE);
    }
    sigact.sa_flags = 0;
    if (sigaction(SIGBUS, &sigact, NULL) == -1) {
        perror("sigaction SIGBUS");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGSEGV, &sigact, NULL) == -1) {
        perror("sigaction SIGSEGV");
        exit(EXIT_FAILURE);
    }

    /* 映射文件 */
    mmap_addr = mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_addr == MAP_FAILED) {
        perror("failed to mmap");
        exit(EXIT_FAILURE);
    }

    /* 访问测试 */
    *(int *)(mmap_addr + file_offset) = 1;
    read_msg = *(int *)(mmap_addr + file_offset);
    printf("0x%x\n", read_msg);

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (munmap(mmap_addr, file_len) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}