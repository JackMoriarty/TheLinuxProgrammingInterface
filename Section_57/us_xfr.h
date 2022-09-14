/**
 * @file us_xfr.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief Unix domain 流socket头文件
 * @version 0.1
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _US_XFR_H_
#define _US_XFR_H_

#include <sys/un.h>
#include <sys/socket.h>

#define SV_SOCK_PATH    "/tmp/us_xfr"

#define BUF_SIZE    100

#endif /* us_xfr.h */