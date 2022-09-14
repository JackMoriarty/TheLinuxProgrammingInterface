/**
 * @file readline.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 按行读取函数头文件
 * @version 0.1
 * @date 2019-07-12
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _READ_LINE_H_
#define _READ_LINE_H_

#define BUF_SIZE    1024

#include <unistd.h>

/* 簿记数据结构 */
struct readline_buf {
    int fd;                 /* 待读取文件的描述符 */
    char buf[BUF_SIZE];     /* 缓冲区 */
    ssize_t buf_len;        /* 缓冲区内有效字符个数 */
    int buf_p;              /* 缓冲器下一个未读取的字符指针 */
};

void readline_buf_init(int fd, struct readline_buf *rlbuf);

ssize_t readline_buf(struct readline_buf *rlbuf, char *buff, size_t n);

#endif   /* readline.h */