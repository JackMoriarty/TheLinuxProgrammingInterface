/**
 * @file my_chattr.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现chattr命令的简化版, 无需实现-R, -V, -v 选项
 * @version 0.1
 * @date 2019-02-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// 输出用法信息
void usage_error();

int main(int argc, char *argv[])
{
    int attr, old_attr, fd;
    // 参数检查
    if (argc < 3 || strchr("+-=", argv[1][0]) == NULL 
        || strcmp(argv[1], "--help") == 0)
        usage_error();
    
    // 获取参数中的相关选项
    attr = 0;
    for (char *p = &argv[1][1]; *p != '\0'; p++) {
        switch(*p){
            case 'a': attr |= FS_APPEND_FL;         break;
            case 'c': attr |= FS_COMPR_FL;          break;
            case 'D': attr |= FS_DIRSYNC_FL;        break;
            case 'i': attr |= FS_IMMUTABLE_FL;      break;
            case 'j': attr |= FS_JOURNAL_DATA_FL;   break;
            case 'A': attr |= FS_NOATIME_FL;        break;
            case 'd': attr |= FS_NODUMP_FL;         break;
            case 't': attr |= FS_NOTAIL_FL;         break;
            case 's': attr |= FS_SECRM_FL;          break;
            case 'S': attr |= FS_SYNC_FL;           break;
            case 'T': attr |= FS_TOPDIR_FL;         break;
            case 'u': attr |= FS_UNRM_FL;           break;
            default: usage_error();
        }
    }

    // 设置选项
    for (int i = 2; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);
        if(fd == -1) {  // 无法打开文件
            fprintf(stderr, "ERROR: Fail to open %s.\n", argv[i]);
            exit(EXIT_FAILURE);
        }

        if (argv[1][0] == '+' || argv[1][0] == '-') {
            // 获取老的标志
            if (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) == -1) {
                fprintf(stderr, "ERROR: fail to get flags from file %s.\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            // 生成新的标志
            attr = (argv[1][0] == '+' ? (old_attr | attr) : (old_attr & ~attr));
        }

        // 设置
        if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1) {
            fprintf(stderr, "ERROR: fail to set flags to file %s.\n", argv[i]);
            exit(EXIT_FAILURE);            
        }

        if(close(fd) == -1) {
            fprintf(stderr, "ERROR: fail to close file %s.\n", argv[i]);
            exit(EXIT_FAILURE);      
        }
    }

    return 0;
}

/**
 * @brief 输出用法信息
 * 
 */
void usage_error()
{
    fprintf(stderr, "Usage: my_chattr {+-=}{attrib-chars} file...\n\n");
    fprintf(stderr, "+ add attribute; - remove attribute; "
                        "= set attributes absolutely\n\n");
    fprintf(stderr, "'attrib-chars' contains one or more of:\n");
    fprintf(stderr, "    a   Force open() to include O_APPEND "
                        "(privilege required)\n");
    fprintf(stderr, "    A   Do not update last access time\n");
    fprintf(stderr, "    c   Compress (requires e2compr package)\n");
    fprintf(stderr, "    d   Do not backup with dump(8)\n");
    fprintf(stderr, "    D   Synchronous directory updates\n");
    fprintf(stderr, "    i   Immutable (privilege required)\n");
    fprintf(stderr, "    j   Enable ext3/ext4 data journaling\n");
    fprintf(stderr, "    s   Secure deletion (not implemented)\n");
    fprintf(stderr, "    S   Synchronous file updates\n");
    fprintf(stderr, "    t   Disable tail-packing (Reiserfs only)\n");
    fprintf(stderr, "    T   Mark as top-level directory for Orlov algorithm\n");
    fprintf(stderr, "    u   Undelete (not implemented)\n");

    exit(EXIT_FAILURE);
}