/**
 * @file my_sendfile.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 利用 write, read 和 lseek 实现 sendfile
 * @version 0.1
 * @date 2019-07-15
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#include <unistd.h>
#include <errno.h>

#define BUF_SIZE    1024

ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    off_t file_offset;
    off_t origin_offset;
    ssize_t trans_num;
    ssize_t num_read;
    ssize_t num_write;
    char buf[BUF_SIZE];
    char *write_buf_start;
    ssize_t tot_writen;
    size_t read_num;
    
    /* 获取当前偏移量 */
    origin_offset = lseek(in_fd, 0, SEEK_CUR);
    if (origin_offset == -1)
        return -1;
    
    /* 移动指针到指定位置 */
    if (offset != NULL) {
        file_offset = lseek(in_fd, *offset, SEEK_CUR);
        if (file_offset == -1)
            return -1;
    }

    /* 传输数据 */
    for (trans_num = 0; trans_num < count; trans_num += num_read) {

        read_num = count - trans_num;
        read_num = read_num > BUF_SIZE ? BUF_SIZE : read_num;
        num_read = read(in_fd, buf, read_num);
        if (num_read == -1) {/* 出错 */
            if (errno == EINTR || errno == EAGAIN)
                continue;
            else
                return -1;
        } else if (num_read == 0) {/* 到达文件尾 */
            break;
        }

        /* 将读到的数据写到输出文件中 */
        write_buf_start = buf;
        for (tot_writen = 0; tot_writen < num_read;) {
            num_write = write(out_fd, write_buf_start, num_read);
            if (num_write == -1) {/* 出错 */
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                else
                    return -1;
            }

            tot_writen += num_write;
            write_buf_start += num_write;
        }
    }

    if (offset != NULL) {
        *offset = lseek(in_fd, 0, SEEK_CUR);
        if (*offset == -1)
            return -1;
    }
    
    if (lseek(in_fd, origin_offset, SEEK_SET) == -1)
        return -1;

    return tot_writen;
}

#ifdef TEST

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd1, fd2;
    int send_num;
    off_t offset;

    fd1 = open("testfile", O_RDONLY);
    fd2 = dup(STDOUT_FILENO);

    send_num = my_sendfile(fd2, fd1, NULL, 5);
    if (send_num == -1)
        perror("my_sendfile");
    else 
        printf("send %ld bytes\n", (long) send_num);

    offset = 3;
    send_num = my_sendfile(fd2, fd1, &offset, 5);
    if (send_num == -1)
        perror("my_sendfile");
    else 
        printf("send %ld bytes with offset %lld\n", (long) send_num, (long long) offset);

    exit(EXIT_SUCCESS);
}

#endif