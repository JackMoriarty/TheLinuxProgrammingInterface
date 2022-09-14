/**
 * @file pty_master_open.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 打开主设备
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _PTY_MASTER_OPEN_H_
#define _PTY_MASTER_OPEN_H_

int ptyMasterOpen(char *slaveName, size_t snLen);

#endif /* pty_master_open.h */