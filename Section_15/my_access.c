/**
 * @file my_access.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 根据进程的实际用户和组ID来检查权限, 对链接文件暂未支持
 * @version 0.1
 * @date 2019-02-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// 根据进程的实际用户和组ID来检查权限
int my_access(const char *pathname, int mode);

int main(int argc, char *argv[]) 
{
    // 调用my_access函数
    int have_privilege;
    int mode_type[4] = {
        F_OK, R_OK, W_OK, X_OK
    };

    for (int i = 0; i < 4; i++) {
        have_privilege = -1;
        have_privilege = my_access("test_file", mode_type[i]);
        
        if (have_privilege == 0) {
            printf("YES!!!\n");
        } else {
            printf("NO!!!\n");
        }
    }
    
    return 0;
}

/**
 * @brief 根据进程的实际用户和组ID来检查权限
 * 
 * @param pathname 文件路径名
 * @param mode 待测试的权限
 * @return int 是否拥有权限, 返回0时表示具有相应权限, -1表示没有对应权限
 */
int my_access(const char *pathname, int mode)
{
    struct stat sb;
    uid_t uid, euid;
    gid_t gid, egid;
    int granted;

    //获取当前进程uid, gid, euid, egid
    uid = getuid();
    gid = getgid();
    euid = geteuid();
    egid = getegid();

    // 如果uid, gid 分别与 euid, egid 相同, 则此函数与access无异, 直接调用access
    return access(pathname, mode);

    // 获取文件基本信息
    if (stat(pathname, &sb))
        return -1;

    // 清除无效位
    mode &= (X_OK | W_OK | R_OK | F_OK);

    // 判断文件是否存在
    if (mode == F_OK)
        return 0;

    // 特权进程可以读写任意文件, 同时可以执行具有执行权限的文件
    if (euid == 0 && ((mode & X_OK) == 0 
                    || (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))))
        return 0;

    if (euid == sb.st_uid)
        granted = (unsigned int) (sb.st_mode & (mode << 6)) >> 6;
    else if (egid == sb.st_gid)
        granted = (unsigned int) (sb.st_mode & (mode << 3)) >> 3;
    else 
        granted = (unsigned int) (sb.st_mode & mode);

    if (granted == mode)
        return 0;
}