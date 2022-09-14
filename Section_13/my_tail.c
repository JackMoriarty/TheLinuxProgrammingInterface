/**
 * @file my_tail.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用I/O系统调用实现tail的功能
 * @version 0.1
 * @date 2019-01-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

extern int optind;
extern char *optarg;

struct content_link_node {
    char content[BUFFER_SIZE];
    int buffer_len;
    struct content_link_node *next;
};

/**
 * @brief 打印指定文件`file_name`末尾`line_size`行
 * 
 * @param file_name 指定的文件名
 * @param line_size 待打印的行数
 */
void read_file_and_output(char *file_name, int line_size)
{
    int buffer_len = 0;
    int fd = 0;
    int space_num = 0;
    struct content_link_node clnh;
    struct content_link_node *cln_p = &clnh;
    struct content_link_node *cln_tmp_p = NULL;
    int index = -1;
    off_t file_head;
    cln_p->next = NULL;


    // 打开指定文件(只读模式)
    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Can not open file(%s).", file_name);
        exit(EXIT_FAILURE);
    }

    // 从文件尾部对'\n'进行计数, 并将读到的数据暂存
    file_head = lseek(fd, 0, SEEK_SET);
    lseek(fd, 0, SEEK_END);
    
    do {
        // 向前移动
        size_t step = BUFFER_SIZE;
        if (lseek(fd, 0, SEEK_CUR) - file_head < BUFFER_SIZE) {
            step = lseek(fd, 0, SEEK_CUR) - file_head;
        }
        lseek(fd, -1 * step, SEEK_CUR);

        buffer_len = 0;
        cln_tmp_p = (struct content_link_node *)malloc(sizeof(struct content_link_node));
        buffer_len = read(fd, cln_tmp_p->content, step);        
        if (buffer_len == -1 || buffer_len != step) {
            fprintf(stderr, "read file (%s) failed.\n", file_name);
            goto clean;
        }

        // 计算换行数量
        if (cln_tmp_p->content[buffer_len] != '\n') {
            space_num ++;
        }
        for (index = buffer_len; index >= 0; index--) {
            if (cln_tmp_p->content[index] == '\n') {
                space_num++;
                if (space_num == line_size + 1) {
                    break;
                }
            }
        }
        // 添加到链表中
        cln_tmp_p->buffer_len = buffer_len - index - 1;
        cln_tmp_p->next = cln_p->next;
        cln_p->next = cln_tmp_p;

        // 游标位置恢复
        lseek(fd, -1 * step, SEEK_CUR);
    } while (lseek(fd, 0, SEEK_CUR) > file_head && space_num < line_size + 1);

    // 输出
    index++;
    write(STDOUT_FILENO, &(cln_tmp_p->content[index]), cln_tmp_p->buffer_len);
    for (cln_tmp_p = cln_tmp_p->next; cln_tmp_p != NULL; cln_tmp_p = cln_tmp_p->next) {
        write(STDOUT_FILENO, cln_tmp_p->content, cln_tmp_p->buffer_len);
    }

clean:
    for (cln_tmp_p = clnh.next; cln_tmp_p != NULL; cln_tmp_p = cln_p) {
        cln_p = cln_tmp_p->next;
        free(cln_tmp_p);
    }
}

int main(int argc, char *argv[])
{
    int opt = 0;
    int line_size = 10;

    // 选项检查
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [ -n num ] file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 选项获取
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        // 错误的选项或缺少参数
        if (opt == '?') {
            fprintf(stderr, "Usage: %s [ -n num ] file\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        int get_num = atoi(optarg);
        line_size = (get_num >= 0 ? get_num : -1 * get_num);
    }

    read_file_and_output(argv[optind], line_size);
    return 0;
}