/*
 * @Description: copy program
 * @Author: Bangduo Chen 
 * @Date: 2018-12-13 10:47:35 
 * @Last Modified by: Bangduo Chen
 * @Last Modified time: 2018-12-13 11:47:17
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* macro */
#define BUFFER_SIZE 1024
/* end of macro */
/**
 * 打印用法信息
 */
static void usage_error(char *program)
{
    fprintf(stderr, "Usage: %s File_1 File_2\n", program);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char *src_file, *des_file;
    int src_fd, des_fd;
    char buffer[BUFFER_SIZE];
    ssize_t read_num, write_num;

    /* 参数检查 */
    if(argc < 3) {
        usage_error(argv[0]);
    }

    src_file = argv[1];
    des_file = argv[2];

    /* 打开两个文件 */
    src_fd = open(src_file, O_RDONLY, S_IRUSR);
    if(src_fd == -1) {
        fprintf(stderr, "Fail to open file:%s\n", src_file);
        exit(EXIT_FAILURE);
    }
    des_fd = open(des_file, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR);
    if(des_fd == -1) {
        fprintf(stderr, "Fail to open file:%s\n", des_file);
        exit(EXIT_FAILURE);
    }

    /* 从源文件中读数据, 写到目的文件中 */
    while((read_num = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        write_num = write(des_fd, buffer, read_num);
        if(write_num != read_num) {
            fprintf(stderr, "Can't write whole buffer\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Copy Success!\n");
    exit(EXIT_SUCCESS);
}