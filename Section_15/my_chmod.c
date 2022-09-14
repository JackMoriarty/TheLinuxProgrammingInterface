/**
 * @file my_chmod.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 使用 stat()和 chmod()编写一程序, 令其等效于执行a+rX命令
 * @version 0.1
 * @date 2019-02-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    struct stat sb;
    mode_t file_mode;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename(OR dirname).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 获取文件权限
    if (stat(argv[1], &sb)) {
        fprintf(stderr, "ERROR: Can not open file(OR dir) %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    file_mode = sb.st_mode;
    
    // 设置读权限
    file_mode |= (S_IRUSR | S_IRGRP | S_IROTH);

    // 设置执行权限
    if (S_ISDIR(file_mode) || file_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        file_mode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }

    // 回写权限
    if (chmod(argv[1], file_mode)) {
        fprintf(stderr, "ERROR: Can not write privilege.\n");
    }
    return 0;
}