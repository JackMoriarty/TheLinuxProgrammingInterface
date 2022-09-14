/*
 * @Description: mytee can read data from stdin and write data in stdout and file
 * @Author: Bangduo Chen 
 * @Date: 2018-12-12 22:31:07 
 * @Last Modified by: Bangduo Chen
 * @Last Modified time: 2018-12-13 10:11:49
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

/* macro */
/* 判断某字符是否能够打印, 如果不能则以'#'代替 */
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')
#define BUFFER_SIZE 1024
/* end of macro */

/* global value */
/* getopt()需要的全局变量 */
extern char *optarg;
extern int optind, opterr, optopt;
/* end of global value */

typedef enum{FALSE, TRUE} Boolean;

/**
 * 传入的选项错误, 打印用法信息并退出
 * @param program   程序名
 * @param msg       错误信息
 * @param opt       非法的选项名
 */
static void usage_error(char *program, char *msg, int opt)
{
    if(msg != NULL && opt != 0) {
        fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
    }
    fprintf(stderr, "Usage: %s [-a] arguments\n", program);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int opt, target_fd;
    ssize_t read_num, write_num;
    char buffer[BUFFER_SIZE];
    int open_flags;
    char *program;

    program = argv[0];
    /* 创建新文件 */
    open_flags = O_RDWR|O_CREAT|O_TRUNC;

    if(argc < 2) {
        usage_error(program, NULL, 0);
    }

    /* 选项解析 */
    while((opt = getopt(argc, argv, ":a")) != -1) {
        if(opt == '?') {
            usage_error(program, "Unrecognized option", opt);
        }

        if(opt == 'a') {
            /* 以追加方式打开文件, 如果没有则创建 */
            open_flags = O_RDWR|O_CREAT|O_APPEND;            
        }
    }

    /* 打开目标文件 */
    target_fd = open(argv[optind], open_flags, S_IRUSR|S_IWUSR);
    if(target_fd == -1) {
        fprintf(stderr, "%s:Fail to open file:%s\n", program, argv[optind]);
        exit(EXIT_FAILURE);
    }

    while(TRUE) {
        /* 从标准输入中读取数据 */
        read_num = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if(read_num == -1) {
            fprintf(stderr, "Fail to read from sdtin");
        }
        /* 将数据写到文件和标准输出中 */
        write_num = write(STDOUT_FILENO, buffer, read_num);
        if (write_num == -1) {
            fprintf(stderr, "%s:Fail to write data in stdin.\n", program);
            exit(EXIT_FAILURE);
        }
        /* 将数据写到文件中 */
        write_num = write(target_fd, buffer, read_num);
        if (write_num == -1) {
            fprintf(stderr, "%s:Fail to write data in File(%s).\n", program, argv[optind]);
            exit(EXIT_FAILURE);
        }
    }
    
    /* 以下不会被执行到 */
    close(target_fd);
    exit(EXIT_SUCCESS);
}