/**
 * @file standard_tty.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 判断当前终端是否为规范模式
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>



int main(int argc, char *argv[])
{
    struct termios tp;

    /* 获取当前终端属性 */
    if (tcgetattr(STDIN_FILENO, &tp) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    if (tp.c_lflag & ICANON) {
        printf("standard mode\n");
    } else {
        printf("non-standard mode: %ld, %ld\n", 
                (long) tp.c_cc[VMIN], tp.c_cc[VTIME]);
    }

    exit(EXIT_SUCCESS);
}