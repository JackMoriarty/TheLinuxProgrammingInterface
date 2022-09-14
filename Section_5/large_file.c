/*
 * @Description: Large file with _FILE_OFFSET_BITS
 * @Author: Bangduo Chen 
 * @Date: 2018-12-15 15:03:30 
 * @Last Modified by: Bangduo Chen
 * @Last Modified time: 2018-12-15 16:39:26
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

/* macro */
#define _FILE_OFFSET_BITS 64
/* end of macro */

void usageErr(const char *format, ...)
{
    va_list argList;

    fflush(stdout);     // 将缓冲在标准输出缓冲区的数据写出

    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);     // 将缓冲在标准错误输出缓冲区中的数据写出]
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int fd;
    off_t off;
    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pathname offset\n", argv[0]);
    }

    fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    if(fd == -1) {
        fprintf(stderr, "fail to open %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    off = atoll(argv[2]);
    if(lseek(fd, off, SEEK_SET) == -1) {
        fprintf(stderr, "fail to lseek at (%lld)\n", (long long)off);
        exit(EXIT_FAILURE);
    }

    if(write(fd, "test", 4) == -1) {
        fprintf(stderr, "fail to write\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}