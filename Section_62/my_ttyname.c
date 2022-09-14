/**
 * @file my_ttyname.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-07-17
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#define FILE_PATH   (PATH_MAX + NAME_MAX)
#define TTY_PATH_NUM    2

static char ttydev_path[TTY_PATH_NUM][PATH_MAX] = {"/dev/", "/dev/pts/"};
static char file_path[FILE_PATH];

char *my_ttyname(int fd)
{
    DIR *dirpath;
    struct dirent *direntp;
    struct stat fd_stat;
    struct stat file_stat;
    int filename_start;
    int i;

    /* 获取参数文描述符文件信息 */
    if (fstat(fd, &fd_stat) == -1)
        return NULL;
    if (!S_ISCHR(fd_stat.st_mode)) {
        errno = ENOTTY;
        return NULL;
    }

    /* 打开终端设备文件夹 */
    for (i = 0; i < TTY_PATH_NUM; i++) {

        strcpy(file_path, ttydev_path[i]);
        filename_start = strlen(ttydev_path[i]);

        dirpath = opendir(ttydev_path[i]);
        if (dirpath == NULL)
            return NULL;

        for (;;) {
            errno = 0;
            direntp = readdir(dirpath);
            if (direntp == NULL) {
                if (errno == 0) /* 查找到达末尾 */
                    break;
                else
                    return NULL;
            }

            /* 拼接文件路径 */
            strcpy(&file_path[filename_start], direntp->d_name);

            /* 获取文件信息 */
            if (lstat(file_path, &file_stat) == -1)
                return NULL;
            
            /* 进行信息比对 */
            if (file_stat.st_rdev == fd_stat.st_rdev)
                return file_path;
        }
    }

    errno = ENOTTY;
    return NULL;
}

#ifdef TEST

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *path;
    int fd;

    fd = open("test", O_CREAT| O_TRUNC | O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    path = my_ttyname(fd);
    if (path != NULL) {
        printf("%s\n", path);
    } else {
        perror("fd");
    }
    close(fd);
    remove("test");

    path = my_ttyname(STDIN_FILENO);
    if (path != NULL)
        printf("%s\n", path);
    else 
        printf("No found\n");

    path = ttyname(STDIN_FILENO);
    if (path != NULL)
        printf("%s\n", path);
    else 
        printf("No found\n");

    exit(EXIT_SUCCESS);
}

#endif /* ifdef TEST */