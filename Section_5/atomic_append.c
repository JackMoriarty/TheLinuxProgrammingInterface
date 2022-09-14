/*
 * @Description: 原子地进行追加
 * @Author: Bangduo Chen 
 * @Date: 2018-12-15 16:33:59 
 * @Last Modified by: Bangduo Chen
 * @Last Modified time: 2018-12-15 22:18:16
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    int o_flags;
    int byte_nums = 0;
    o_flags = O_WRONLY|O_CREAT;

    /* 参数检查 */
    if(argc < 3 || argc > 4 || (argc == 4  && strcmp(argv[3], "x"))) {
        fprintf(stderr, "Usage: %s filename num-bytes [x]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* 当存在x时, 不使用追加标志, 否则使用 */
    if(argc != 4) {
        o_flags |= O_APPEND;
    }
    byte_nums = atoi(argv[2]);

    /* 打开文件 */
    fd = open(argv[1], o_flags, S_IWUSR|S_IRUSR);
    if (fd == -1) {
        fprintf(stderr, "fail to open file(%s)\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    /* 写入数据 */
    if(argc == 4 && strcmp(argv[3], "x") == 0) {
        for(int i = 0; i < byte_nums; i++) {
            if(lseek(fd, 0, SEEK_END) == -1) {
                fprintf(stderr, "fail to lseek file(%s)\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            if(write(fd, "x", 1) == -1) {
                fprintf(stderr, "fail to write file(%s)\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }
    } else {
        // 使用追加模式
        for(int i = 0; i < byte_nums; i++) {
            if(write(fd, "x", 1) == -1) {
                fprintf(stderr, "fail to write file(%s)\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}