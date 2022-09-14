/**
 * @file ud_ucase.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief unix domain 数据报socket头文件
 * @version 0.1
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _UD_UCASE_H
#define _UD_UCASE_H

#include <sys/un.h>
#include <sys/socket.h>

#define BUF_SIZE    10                          /* 消息缓冲大小 */
#define SV_SOCK_PATH    "/tmp/ud_ucase"         /* socket 地址 */

#endif /* ud_ucase.h */