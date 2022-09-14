/**
 * @file my_t_stat.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 提供纳秒级的精度来显示文件时间戳
 * @version 0.1
 * @date 2019-02-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>

#define STR_SIZE sizeof("rwxrwxrwx")

// 打印struct stat结构体中的信息
void display_stat_info(const struct stat *sb);
// 将文件权限掩码转化为字符串
char *file_perm_str(mode_t perm, int flags);

int main(int argc, char *argv[])
{
    struct stat sb;     // 文件属性结构体
    char stat_link;     // 如果定义了`-l`选项, 则使用lstat
    int fname;          // 文件名位置

    // 设置stat_link的值及fname的位置
    stat_link = (argc > 1) && strcmp(argv[1], "-l") == 0;
    fname = stat_link ? 2 : 1;

    if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "%s [-l] file\n"
                        "       -l = use lstat() instead of stat()\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat_link) {
        if (lstat(argv[fname], &sb) == -1) {
            fprintf(stderr, "Error in lstat().\n");
            exit(EXIT_FAILURE);
        }
    } else {
        if (stat(argv[fname], &sb) == -1) {
            fprintf(stderr, "Error in stat().");
            exit(EXIT_FAILURE);
        }
    }

    display_stat_info(&sb);

    return 0;
}

/**
 * @brief       打印struct stat结构体中的信息 
 * 
 * @param sb    文件属性结构体
 */
void display_stat_info(const struct stat *sb)
{
    printf("file type:              ");

    switch(sb->st_mode & S_IFMT) {
        case S_IFREG:  printf("regular file\n");                break;
        case S_IFDIR:  printf("directory\n");                   break;
        case S_IFCHR:  printf("character device\n");            break;
        case S_IFBLK:  printf("block device\n");                break;
        case S_IFLNK:  printf("symbolic (soft) link\n");        break;
        case S_IFIFO:  printf("FIFO or pipe\n");                break;
        case S_IFSOCK: printf("socket\n");                      break;
        default:       printf("unknown file type?\n");          break;
    }

    printf("Device containing i-node: major=%ld minor=%ld\n",
                (long)major(sb->st_dev), (long)minor(sb->st_dev));

    printf("I-node number:            %ld", (long)(sb->st_ino));

    printf("Mode:                     %lo (%s)\n",
            (unsigned long)(sb->st_mode), file_perm_str(sb->st_mode, 0));

    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
        printf("    special bits set:   %s%s%s\n",
                (sb->st_mode & S_ISUID) ? "set-UID " : "",
                (sb->st_mode & S_ISGID) ? "set-GID " : "",
                (sb->st_mode & S_ISVTX ? "sticky ": ""));

    printf("Number of (hard) links:   %ld\n", (long)(sb->st_nlink));

    printf("Ownership:                UID=%ld   GID=%ld\n",
            (long)(sb->st_uid), (long)(sb->st_gid));

    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
        printf("Device number (st_rdev): major=%ld; minor=%ld\n",
                (long) major(sb->st_rdev), (long) minor(sb->st_rdev));
    
    printf("File size:                %lld bytes\n", (long long)(sb->st_size));
    printf("Optimal I/O block size:   %ld bytes\n", (long)(sb->st_blksize));
    printf("512B blocks allocated:    %lld\n", (long long)(sb->st_blocks));
    printf("Last file access:         %s%ld(ns)\n", ctime(&(sb->st_atime)), (long)((sb->st_atim).tv_nsec));
    printf("Last file modification:   %s%ld(ns)\n", ctime(&(sb->st_mtime)), (long)((sb->st_mtim).tv_nsec));
    printf("Last status change:       %s%ld(ns)\n", ctime(&(sb->st_ctime)), (long)((sb->st_ctim).tv_nsec));
}

/**
 * @brief 将文件权限掩码转化为字符串
 * 
 * @param perm 文件权限掩码
 * @param flags 是否要包含SUID, GUID 和 Sticky位
 * @return char* 权限字符串
 */
char *file_perm_str(mode_t perm, int flags)
{
    static char str[STR_SIZE];

    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
            (perm & S_IRUSR) ? 'r' : '-',
            (perm & S_IWUSR) ? 'w' : '-',
            (perm & S_IXUSR) ? (((perm & S_ISUID) && flags) ? 's' : 'x') :
                               (((perm & S_ISUID) && flags) ? 'S' : '-'),
            (perm & S_IRGRP) ? 'r' : '-',
            (perm & S_IWGRP) ? 'w' : '-',
            (perm & S_IXGRP) ? (((perm & S_ISGID) && flags) ? 's' : 'x') :
                               (((perm & S_ISGID) && flags) ? 'S' : '-'),
            (perm & S_IROTH) ? 'r' : '-',
            (perm & S_IWOTH) ? 'w' : '-',
            (perm & S_IXOTH) ? (((perm & S_ISVTX) && flags) ? 't' : 'x') :
                               (((perm & S_ISVTX) && flags) ? 'T' : '-')
            );
    
    return str;
}