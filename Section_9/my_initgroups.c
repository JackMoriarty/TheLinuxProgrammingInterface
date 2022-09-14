/**
 * @file my_initgroups.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 使用setgroups和库函数实现initgroups.
 * @version 0.1
 * @date 2019-01-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

gid_t grouplist[NGROUPS_MAX + 1];
int grouplist_size = 0;

/**
 * @brief 初始化辅助组ID, 同时将group追加到辅助组ID中
 * 
 * @param user 用户名
 * @param group 指定的组ID
 * @return int 成功返回0, 失败返回-1
 */
int my_initgroups(const char *user, gid_t group)
{
    struct passwd *user_msg = NULL;;
    struct group *group_p = NULL;
    
    // 从密码文件中获取相应用户组ID
    user_msg = getpwnam(user);
    if (user_msg == NULL) {
        // 没有该用户
        return -1;
    }

    // 存在该用户, 添加密码文件中用户所属组ID
    grouplist[grouplist_size++] = user_msg->pw_gid;

    // 从组文件中获取包含该用户的组ID(遍历)
    while ((group_p = getgrent()) != NULL) {
        char **user_name = group_p->gr_mem;
        for (;*user_name != NULL; user_name++) {
            if (strcmp(*user_name, user) == 0) {
                // 该组存在该用户
                grouplist[grouplist_size++] = group_p->gr_gid;
                break;
            }
        }
    }

    // 追加指定的组ID
    grouplist[grouplist_size++] = group;

    // 替换调用进程的辅助组ID
    int retval = 0;
    retval = setgroups(grouplist_size, grouplist);

    return retval;
}

void print_process_group_id()
{
    // 获取并打印当前用户辅助组ID
    gid_t gid_list[NGROUPS_MAX + 1];
    int gid_size = 0;
    
    gid_size = getgroups(NGROUPS_MAX + 1, gid_list);
    if (gid_size == -1) {
        fprintf(stderr, "can't get group_id!\n");
        exit(EXIT_FAILURE);
    }

    printf("gid_list is");
    for (int i = 0; i < gid_size; i++) {
        printf(":%d", gid_list[i]);
    }
    putchar('\n');
}

int main(int argc, char *argv[])
{
    print_process_group_id();
    int retval = my_initgroups("chenbangduo", 666);
    if (retval == -1) {
        printf("init failed!\n");
    } else {
        print_process_group_id();
    }
    
    return 0;
}