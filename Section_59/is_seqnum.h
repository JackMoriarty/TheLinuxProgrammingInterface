/**
 * @file is_seqnum.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 序号服务器和客户端共享头文件
 * @version 0.1
 * @date 2019-07-12
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _IS_SEQNUM_H_
#define _IS_SEQNUM_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "readline.h"

#define PORT_NUM    "50000"         /* 服务器端口号 */
#define INT_LEN     50              /* 字符串能够完整包含整数表示的长度, 包含'\n' */

#endif  /* is_seqnum.h */