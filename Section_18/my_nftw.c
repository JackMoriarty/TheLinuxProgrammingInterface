/**
 * @file my_ntfw.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现ntfw函数
 * @version 0.1
 * @date 2019-03-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <linux/limits.h>
#include <fcntl.h>

int my_nftw(const char *dirpath, 
    int (*func) (const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf),
    int nopenfd, int flags)
{
    struct stat statbuf;
    struct FTW ftw_buf;
    int type_flags = 0;
    static int level = 0;
    DIR *dir_p = NULL;
    struct dirent *dir_msg;
    int func_retval = 0;
    static dev_t devno;

    // 填充ftw_buf
    int index = strlen(dirpath);
    while(index > 0 && dirpath[--index] != '/');
    ftw_buf.base = index;
    ftw_buf.level = level;

    // 检查当前路径类型
    if (flags & FTW_PHYS) {
        if (lstat(dirpath, &statbuf) == -1) {
            type_flags = FTW_NS;
        }        
    } else {
        if (stat(dirpath, &statbuf) == -1) {
            type_flags = FTW_NS;
        }
    }

    // 跨越文件系统
    if (flags & FTW_MOUNT) {
        if (level == 0) devno = statbuf.st_dev;
        if (devno != statbuf.st_dev) return 0;
    }

    if (type_flags == 0) {
        if (S_ISDIR(statbuf.st_mode)) {
            // 文件夹
            // 打开当前文件夹, 给type_flags赋值
            if ((dir_p = opendir(dirpath)) == NULL) {
                type_flags = FTW_DNR;
            } else {
                type_flags = FTW_D;
            }
            if ((flags & FTW_DEPTH) == 0) { // 先序遍历
                func_retval = func(dirpath, &statbuf, type_flags, &ftw_buf);
                if (func_retval) {
                    closedir(dir_p);
                    return func_retval;
                }
            }
            // 遍历当前目录下的项目, 递归调用
            if (type_flags == FTW_D) {
                while (1) {
                    errno = 0;
                    int local_type_flags = 0;
                    dir_msg = readdir(dir_p);
                    // puts(dir_msg->d_name);
                    if (dir_msg == NULL) {
                        if (errno != 0) {
                            closedir(dir_p);
                            return -1;
                        } else {
                            break;
                        }
                    }
                    // 拼接文件夹下项目路径, 跳过 ".." 和 "."
                    if (strcmp("..", dir_msg->d_name) != 0 && strcmp(".", dir_msg->d_name) != 0) {
                        char local_pathname[PATH_MAX];
                        char cwd[PATH_MAX];
                        int my_ntfw_retval = 0;
                        int fd = 0;
                        struct stat local_statbuf;

                        strcpy(local_pathname, dirpath);
                        strcat(local_pathname, "/");
                        strcat(local_pathname, dir_msg->d_name);
                        level++;
                        if (flags & FTW_PHYS) {
                            if (lstat(local_pathname, &local_statbuf) == -1) {
                                local_type_flags = FTW_NS;
                            }        
                        } else {
                            if (stat(local_pathname, &local_statbuf) == -1) {
                                local_type_flags = FTW_NS;
                            }
                        }
                        int need_chdir = flags & FTW_CHDIR && S_ISDIR(local_statbuf.st_mode);
                        if (local_type_flags != FTW_NS && need_chdir) {
                            if (getcwd(cwd, PATH_MAX) == NULL) {
                                return -1;
                            }
                            if ((fd = open(cwd, S_IRUSR)) == -1) {
                                return -1;
                            }
                            if (chdir(local_pathname) == -1) {
                                return -1;
                            }
                            // puts(getcwd(cwd, PATH_MAX));
                        }
                        my_ntfw_retval = my_nftw(local_pathname, func, nopenfd, flags);
                        level--;
                        if (local_type_flags != FTW_NS && need_chdir) {
                            if (fchdir(fd) == -1) {
                                return -1;
                            }
                            close(fd);
                            // puts(getcwd(cwd, PATH_MAX));
                        }
                        if (my_ntfw_retval != 0) {
                            closedir(dir_p);
                            return my_ntfw_retval;
                        }
                    }
                }
            }
            if (flags & FTW_DEPTH) { // 后序遍历
                func_retval = func(dirpath, &statbuf, type_flags, &ftw_buf);
                if (func_retval) {
                    closedir(dir_p);
                    return func_retval;
                }
            }
            if (closedir(dir_p) == -1) return -1;
        } else {
            // 非文件夹
            if (flags & FTW_PHYS && S_ISLNK(statbuf.st_mode)) {
                // 不启用符号链接解用
                // 判断是否是悬空链接
                struct stat lnk_statbuf;
                if (stat(dirpath, &lnk_statbuf) == -1) {
                    if (errno == ENOENT) {
                        type_flags = FTW_SLN;
                    } else {
                        return -1;
                    }
                } else {
                    type_flags = FTW_SL;
                }
            } else {
                type_flags = FTW_F;
            }
            // 文件, 无后序遍历的区别, 直接调用回调函数
            func(dirpath, &statbuf, type_flags, &ftw_buf);
        }
    } else {
        // 无法获取当前路径信息, 直接调用回调函数
        func(dirpath, &statbuf, type_flags, &ftw_buf);
    }

    return 0;
}

int my_func(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) {
    printf("%-50s:", pathname);
    switch (typeflag) {
        case FTW_D: printf("FTW_D"); break;
        case FTW_DNR: printf("FTW_DNR"); break;
        case FTW_DP: printf("FTW_DP"); break;
        case FTW_F: printf("FTW_F"); break;
        case FTW_NS: printf("FTW_NS"); break;
        case FTW_SL: printf("FTW_SL"); break;
        case FTW_SLN: printf("FTW_SLN"); break;
        default: break;
    }
    char cwdbuf[PATH_MAX];
    printf(":%s", getcwd(cwdbuf, PATH_MAX));
    puts("");
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s path.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    my_nftw(argv[1], my_func, 10, FTW_CHDIR);

    return 0;
}