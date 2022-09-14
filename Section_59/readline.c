/**
 * @file readline.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 实现按行读取的函数
 * @version 0.1
 * @date 2019-07-12
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <errno.h>
#include "readline.h"

/**
 * @brief 初始化readline簿记结构
 * 
 * @param fd 待读取文件的文件描述符
 * @param rlbuf readline簿记结构
*/
void readline_buf_init(int fd, struct readline_buf *rlbuf)
{
    rlbuf->fd = fd;
    rlbuf->buf_len = 0;
    rlbuf->buf_p = 0;
}

/**
 * @brief 将一行的数据放置到buff中, 如果buff大小不够, 则对数据进行截断
 * 
 * @param rlbuf 簿记结构
 * @param buff 接收缓冲区
 * @param n 接收缓冲区大小
 * @return ssize_t 成功时缓冲区中字符数量, 0表示文件已达末尾, 否则返回-1;
*/
ssize_t readline_buf(struct readline_buf *rlbuf, char *buff, size_t n)
{
    size_t cnt;
    char c;

    if (n <= 0 || buff == NULL) {
        errno = EINVAL;
        return -1;
    }

    cnt = 0;

    for (;;) {
        if (rlbuf->buf_p >= rlbuf->buf_len) {
            rlbuf->buf_len = read(rlbuf->fd, rlbuf->buf, BUF_SIZE);
            if (rlbuf->buf_len == -1)
                return -1;
            
            if (rlbuf->buf_len == 0)
                break;
            
            rlbuf->buf_p = 0;
        }

        c = rlbuf->buf[rlbuf->buf_p];
        rlbuf->buf_p++;

        if (cnt < n)
            buff[cnt++] = c;
        
        if (c == '\n')
            break;
    }

    return cnt;
}