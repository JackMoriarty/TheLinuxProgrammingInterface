/**
 * @file nftw_use.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 遍历文件树
 * @version 0.1
 * @date 2019-03-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/stat.h>

int reg = 0;
int dir = 0;
int chr = 0;
int blk = 0;
int lnk = 0;
int fifo = 0;
int sock = 0;

static int dir_tree(const char *pathname, const struct stat *sbuf, int type, 
            struct FTW *ftwb)
{
    // puts(pathname);
    switch (sbuf->st_mode & S_IFMT)
    {
        case S_IFREG: reg++; break;
        case S_IFDIR: dir++; break;
        case S_IFCHR: chr++; break;
        case S_IFBLK: blk++; break;
        case S_IFLNK: lnk++; break;
        case S_IFIFO: fifo++; break;
        case S_IFSOCK: sock++; break;
        default:
            break;
    }
    return 0;
}
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (nftw(argv[1], dir_tree, 10, FTW_PHYS) == -1) {
        perror("ntfw");
        exit(EXIT_FAILURE);
    }

    int sum = reg + dir + chr + blk + lnk + fifo + sock;
    printf("NUM: %d\n", sum);
    printf("REG:  %.2lf\n", reg * 100.0 / sum);
    printf("DIR:  %.2lf\n", dir * 100.0 / sum);
    printf("CHR:  %.2lf\n", chr * 100.0 / sum);
    printf("BLK:  %.2lf\n", blk * 100.0 / sum);
    printf("LNK:  %.2lf\n", lnk * 100.0 / sum);
    printf("FIFO: %.2lf\n", fifo * 100.0 / sum);
    printf("SOCK: %.2lf\n", sock * 100.0 / sum);
    
    return 0;
}