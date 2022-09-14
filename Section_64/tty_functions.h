/**
 * @file tty_functions.h
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 切换终端到原始模式
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#ifndef _TTY_FUNCTIONS_H_
#define _TTY_FUNCTIONS_H_

int ttySetRaw(int fd, struct termios *prevTermios);

#endif /* tty_funcions.h */