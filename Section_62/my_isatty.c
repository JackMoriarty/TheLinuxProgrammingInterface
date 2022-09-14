/**
 * @file my_isatty.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 实现 isatty 函数的功能
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <termios.h>

int my_isatty(int fd)
{
    struct termios tios;
    int retval;

    retval = tcgetattr(fd, &tios);

    return retval;
}

#ifdef TEST

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    printf("%s\n", my_isatty(STDIN_FILENO) == 0 ? "Yes" : "No");
    fd = open("testfile", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    printf("%s\n", my_isatty(fd) == 0 ? "Yes" : "No");
    remove("testfile");
    close(fd);
    exit(EXIT_SUCCESS);
}

#endif  /* ifdef TEST */