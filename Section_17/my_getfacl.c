/**
 * @file my_getfacl.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 根据特定的用户或组相对应的ACE来显示权限
 * @version 0.1
 * @date 2019-02-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <acl/libacl.h>
#include <pwd.h>
#include <grp.h>

void print_perm(acl_permset_t acl_perm, const acl_permset_t acl_mask, 
        const int mask_exist);
uid_t uid_from_username(const char *username);
gid_t gid_from_groupname(const char *groupname);

int main(int argc, char *argv[])
{
    char path_name[5] = "a";
    struct stat statbuf;
    acl_t acl;
    acl_type_t acl_type;
    acl_tag_t tag;
    int is_user = 0;
    acl_entry_t entry;
    int entry_id;
    int is_own_user_group = 0;
    long u_g_id;
    acl_permset_t acl_mask;
    acl_permset_t acl_perm;
    acl_permset_t acl_other_perm;
    int u_g_exist = 0;
    int mask_exist = 0;

    // 参数检查
    if (argc != 3 || (strcmp("u", argv[1]) && strcmp("g", argv[1]))) {
        fprintf(stderr, "Usage: %s [u|g] [username|group].\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 获取文件类型, 填充acl_type, 获取文件属主和属组id
    if (stat(path_name, &statbuf)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    if (S_ISREG(statbuf.st_mode)) {
        acl_type = ACL_TYPE_ACCESS;
    } else if (S_ISDIR(statbuf.st_mode)) {
        acl_type = ACL_TYPE_DEFAULT;
    } else {
        fprintf(stderr, "Not regular file or directory.\n");
        exit(EXIT_FAILURE);
    }

    // 获取参数中的用户id或组id, 并判断是否是文件属主或属组
    is_user = strcmp("u", argv[1]) ? 0 : 1;
    
    if (is_user) {
        u_g_id = uid_from_username(argv[2]);
        is_own_user_group = u_g_id == statbuf.st_uid ? 1 : 0;
    } else {
        u_g_id = gid_from_groupname(argv[2]);
        is_own_user_group = u_g_id == statbuf.st_gid ? 1 : 0;
    }

    // 读入acl到内存并获得记录
    acl = acl_get_file(path_name, acl_type);
    if (acl == NULL) {
        perror("acl_get_file");
        exit(EXIT_FAILURE);
    }

    for (entry_id = ACL_FIRST_ENTRY; ; entry_id = ACL_NEXT_ENTRY) {
        int retval = acl_get_entry(acl, entry_id, &entry);
        if (retval == 1) {
            // 获取标记类型
            if (acl_get_tag_type(entry, &tag) == -1) {
                perror("acl_get_tag_type");
                exit(EXIT_FAILURE);
            }

            if(tag == ACL_USER_OBJ && is_user && is_own_user_group) {
                // 取得文件属主的权限
                if (acl_get_permset(entry, &acl_perm) == -1) {
                    perror("acl_get_permset in ACL_USER_OBJ");
                    exit(EXIT_FAILURE);
                }
                u_g_exist = 1;
            } else if (tag == ACL_USER && is_user) {
                // 获取用户id
                uid_t *ace_uid;
                ace_uid = acl_get_qualifier(entry);
                if (ace_uid == NULL) {
                    perror("acl_get_qualifier in ACL_USER");
                    exit(EXIT_FAILURE);
                }
                
                if (*ace_uid == (uid_t) u_g_id) {
                    // 获取该用户权限
                    if (acl_get_permset(entry, &acl_perm) == -1) {
                        perror("acl_get_permset in ACL_USER");
                        exit(EXIT_FAILURE);
                    }
                }
                if (acl_free(ace_uid) == -1) {
                    perror("acl_free in ACL_USER");
                    exit(EXIT_FAILURE);
                }
                u_g_exist = 1;
            } else if (tag == ACL_GROUP_OBJ && !is_user && is_own_user_group) {
                // 取得文件所属组权限
                if (acl_get_permset(entry, &acl_perm) == -1) {
                    perror("acl_get_permset in ACL_GROUP_OBJ");
                    exit(EXIT_FAILURE);
                }
                u_g_exist = 1;
            } else if (tag == ACL_GROUP && !is_user) {
                // 获取组id
                gid_t *ace_gid;
                ace_gid = acl_get_qualifier(entry);
                if (ace_gid == NULL) {
                    perror("acl_get_qualifier in ACL_GROUP");
                    exit(EXIT_FAILURE);
                }

                if (*ace_gid == (gid_t) u_g_id) {
                    // 获取该组权限
                    if (acl_get_permset(entry, &acl_perm) == -1) {
                        perror("acl_get_permset in ACL_GROUP");
                        exit(EXIT_FAILURE);
                    }
                }
                if (acl_free(ace_gid) == -1) {
                    perror("acl_free in ACL_GROUP");
                    exit(EXIT_FAILURE);
                }
                u_g_exist = 1;
            } else if (tag == ACL_MASK) {
                // 获取掩码
                if (acl_get_permset(entry, &acl_mask) == -1) {
                    perror("acl_get_permset in ACL_MASK");
                    exit(EXIT_FAILURE);
                }
                mask_exist = 1;
            } else if (tag == ACL_OTHER){
                // 获取其他的权限
                if (acl_get_permset(entry, &acl_other_perm) == -1) {
                    perror("acl_get_permset in ACL_OTHER");
                    exit(EXIT_FAILURE);
                }
            }
        } else if (retval == 0) {
            break;
        } else if (retval == -1) {
            perror("acl_get_entry");
            exit(EXIT_FAILURE);
        }
    }

    // 输出权限
    if (u_g_exist) {
        if(is_user && is_own_user_group) {
            print_perm(acl_perm, acl_mask, 0);
        } else {
            print_perm(acl_perm, acl_mask, mask_exist);
        }
    } else {
        print_perm(acl_other_perm, acl_mask, 0);
    }

    return 0;
}

/**
 * @brief 将权限转化为`rwx-`形式
 * 
 * @param acl_perm 权限
 */
void perm_convert(const acl_permset_t acl_perm, const acl_permset_t acl_mask,
        const int mask_use)
{
    int perm_val = 0;
    int mask_perm_val = 0;
    acl_perm_t perm_type[3] = {
        ACL_READ, ACL_WRITE, ACL_EXECUTE
    };
    char acl_perm_rwx[3] = {'r', 'w', 'x'};
    
    for (int i = 0; i < 3; i++){
        perm_val = acl_get_perm(acl_perm, perm_type[i]);
        // printf("[%d:%d]", perm_val, perm_type[i]);
        if (mask_use) mask_perm_val = acl_get_perm(acl_mask, perm_type[i]);
        if (perm_val == -1 || mask_perm_val == -1) {
            // printf("%d:%d\n", perm_val, mask_perm_val);
            perror("acl_get_perm");
            exit(EXIT_FAILURE);
        }
        printf("%c", (perm_val == 1 && (mask_use ? mask_perm_val : 1) == 1) ? acl_perm_rwx[i] : '-');
    }
}

/**
 * @brief 打印权限
 * 
 * @param acl_perm 权限
 * @param acl_mask 掩码
 * @param mask_exist 掩码是否存在
 */
void print_perm(acl_permset_t acl_perm, const acl_permset_t acl_mask, 
        const int mask_exist)
{
    // 输出原有权限
    perm_convert(acl_perm, acl_mask, 0);

    if (mask_exist) {
        printf("%20s", "#effictive:");
        perm_convert(acl_perm, acl_mask, 1);
    }
    putchar('\n');
}

/**
 * @brief 字符串转uid
 * 
 * @param username 用户名
 * @return uid_t 用户id
 */
uid_t uid_from_username(const char *username)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (username == NULL || *username == '\0') {
        fprintf(stderr, "username not exist!");
        exit(EXIT_FAILURE);
    }

    u = strtol(username, &endptr, 10);
    if (*endptr == '\0') return u;

    pwd = getpwnam(username);
    if (pwd == NULL) {
        perror("getpwnam");
        exit(EXIT_FAILURE);
    }

    return pwd->pw_uid;
}

/**
 * @brief 字符串转gid
 * 
 * @param groupname 组名
 * @return gid_t 组id
 */
gid_t gid_from_groupname(const char *groupname)
{
    struct group *grp;
    gid_t g;
    char *endptr;

    if (groupname == NULL || *groupname == '\0') {
        fprintf(stderr, "groupname not exist!");
        exit(EXIT_FAILURE);
    }

    g = strtol(groupname, &endptr, 10);
    if (*endptr == '\0') return g;

    grp = getgrnam(groupname);
    if (grp == NULL) {
        perror("getgrnam");
        exit(EXIT_FAILURE);
    }

    return grp->gr_gid;
}