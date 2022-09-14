/**
 * @file svshm_xfr.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief system V 共享内存读写者程序头文件
 * @version 0.1
 * @date 2019-07-04
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _SVSHM_XFR_H_
#define _SVSHM_XFR_H_

#include <sys/types.h>
#include <sys/stat.h>

#define SHM_KEY     0x1234              /* 共享内存段 key */
#define SEM_WRITE   "/mq-write"         /* 写信号量名称 */
#define SEM_READ    "/mq-read"          /* 读信号量名称 */

#define OBJ_PERMS   (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
                                        /* IPC 权限 */

#ifndef BUF_SIZE                        /* 允许使用编译时声明 */
#define BUF_SIZE    1024                /* 共享内存大小 */
#endif /* BUF_SIZE */

/* 定义共享内存结构体 */
struct shmseg {
    int cnt;                            /* buf 使用的字节数量 */
    char buf[BUF_SIZE];                 /* 数据 */
};

#endif /* svshm_xfr.h */