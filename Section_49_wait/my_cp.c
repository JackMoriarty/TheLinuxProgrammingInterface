/**
 * @file my_cp.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 使用内存映射技术实现cp命令的功能
 * @version 0.1
 * @date 2019-07-06
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{   
    int src_fd, dst_fd;
    int srcfile_len;
    struct stat src_statbuf;
    void *src_mmap_addr, *dst_mmap_addr;
    int exit_status = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s srcfile dstfile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 打开源文件并获取源文件相关信息 */
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("open srcfile");
        exit(EXIT_FAILURE);
    }
    if (fstat(src_fd, &src_statbuf) == -1) {
        perror("srcfile fstat");
        exit(EXIT_FAILURE);
    }
    srcfile_len = src_statbuf.st_size;
    /* 创建源文件内存映射 */
    src_mmap_addr = mmap(NULL, srcfile_len, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_mmap_addr == MAP_FAILED) {
        perror("mmap srcfile");
        exit(EXIT_FAILURE);
    }

    /* 打开目标文件并设置文件相关信息 */
    dst_fd = open(argv[2], O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP);
    if (dst_fd == -1) {
        perror("open dstfile");
        exit(EXIT_FAILURE);
    }
    /* 更改目标文件长度 */
    if (ftruncate(dst_fd, srcfile_len) == -1) {
        perror("change dstfile length");
        exit(EXIT_FAILURE);
    }
    /* 创建目标文件内存映射 */
    dst_mmap_addr = mmap(NULL, srcfile_len, PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst_mmap_addr == MAP_FAILED) {
        perror("mmap dstfile");
        exit(EXIT_FAILURE);
    }

    /* 内存复制 */
    memcpy(dst_mmap_addr, src_mmap_addr, srcfile_len);

    /* 解除文件映射 */
    if (munmap(src_mmap_addr, srcfile_len) == -1) {
        exit_status++;
        perror("munmap srcfile");
    }
    if (munmap(dst_mmap_addr, srcfile_len) == -1) {
        exit_status++;
        perror("munmap dstfile");
    }
    if (close(src_fd) == -1) {
        exit_status++;
        perror("close srcfd");
    }
    if (close(dst_fd) == -1) {
        exit_status++;
        perror("close dstfd");
    }
    if (exit_status != 0)
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}