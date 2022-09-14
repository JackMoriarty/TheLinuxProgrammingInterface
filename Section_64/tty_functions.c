/**
 * @file tty_functions.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 将终端设置为原始模式
 * @version 0.1
 * @date 2019-07-20
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <termios.h>
#include <stdlib.h>

int ttySetRaw(int fd, struct termios *prevTermios)
{
    struct termios t;

    if (tcgetattr(fd, &t) == -1)
        return -1;

    if (prevTermios != NULL)
        *prevTermios = t;
    
    t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR |
                    INPCK | ISTRIP | IXON | PARMRK);
    t.c_oflag &= ~OPOST;

    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
        return -1;
    
    return 0;
}