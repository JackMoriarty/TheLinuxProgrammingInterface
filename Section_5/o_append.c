/*
 * @Description: 以追加方式打开文件并在文件头开始写入数据
 * @Author: Bangduo Chen 
 * @Date: 2018-12-15 16:16:53 
 * @Last Modified by: Bangduo Chen
 * @Last Modified time: 2018-12-15 16:39:43
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd;

    /* 参数检查 */
    if(argc < 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 使用O_APPEND标志并以写的方式打开文件 */
    fd = open(argv[1], O_WRONLY, S_IRUSR|S_IWUSR);
    if(fd == -1) {
        fprintf(stderr, "fail to open file:%s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* 将文件偏移量移到文件头部 */
    if(lseek(fd, 0, SEEK_SET) == -1) {
        fprintf(stderr, "fail to lseek\n");
        exit(EXIT_FAILURE);
    }

    /* 写入数据 */
    if(write(fd, "test", 4) == -1) {
        fprintf(stderr, "fail to write\n");
        exit(EXIT_FAILURE);
    }
    
    close(fd);
    exit(EXIT_SUCCESS);
}