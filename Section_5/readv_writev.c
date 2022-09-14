/**
 * @file readv_writev.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用read和write以及malloc函数包实现readv和writev
 * @version 0.1
 * @date 2018-12-16
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct iovec{
    void *iov_base;
    size_t iov_len;
};

/**
 * @brief 分散输入
 * 
 * @param fd        文件描述符
 * @param iov       缓冲区向量
 * @param iovcnt    缓冲区数目
 * @return ssize_t  读到的字节数, EOF或-1
 */
ssize_t myreadv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t read_num = 0;
    for(int i = 0; i < iovcnt; i++) {
        ssize_t tmp_num = read(fd, iov[i].iov_base, iov[i].iov_len);
        if(tmp_num == -1) {
            return -1;
        }
        read_num += tmp_num;
    }

    return read_num;
}

/**
 * @brief 集合输入
 * 
 * @param fd        文件描述符
 * @param iov       缓冲区向量
 * @param iovcnt    缓冲区数目
 * @return ssize_t  读到的字节数, EOF或-1
 */
ssize_t mywritev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t write_num = 0;
    for(int i = 0; i < iovcnt; i++) {
        ssize_t tmp_num = write(fd, iov[i].iov_base, iov[i].iov_len);
        if(tmp_num == -1) {
            return -1;
        }
        write_num += tmp_num;
    }

    return write_num;
}