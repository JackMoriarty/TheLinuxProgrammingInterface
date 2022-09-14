/**
 * @file my_setfattr.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief setfattr的简化版, 可创建或修改文件的`user EA`
 * @version 0.1
 * @date 2019-02-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>

extern int optind, optopt;
extern char *optarg;

void usage_err();

int main(int argc, char *argv[])
{
    char *ea_name = NULL;
    char *ea_value = NULL;
    char *file_path = NULL;
    int opt;
    int n_or_x = 0;

    // 参数检查
    if (argc < 4 || argc > 6) {
        // 参数数量不足或过多
        usage_err();
    }

    // 参数解析
    while((opt = getopt(argc, argv, "n:x:v:")) != -1) {
        if (opt == '?' || opt ==':') {
            // 未识别或没提供参数
            usage_err();
        }

        switch (opt)
        {
            case 'n':
                n_or_x = 0;
                ea_name = optarg;
                break;

            case 'x':
                n_or_x = 1;
                ea_name = optarg;
                break;
            
            case 'v':
                ea_value = optarg;
                break;

            default:
                usage_err();
                break;
        }
    }
    file_path = argv[optind];

    // 判断文件是否存在
    if (access(file_path, F_OK)) {
        fprintf(stderr, "File not exist!\n");
        exit(EXIT_FAILURE);
    }

    // 对user EA 进行操作
    if (n_or_x == 0) {
        if(setxattr(file_path, ea_name, ea_value, strlen(ea_value) + 1, 0)) {
            fprintf(stderr, "Fail to set (name:%s value:%s).\n", ea_name, ea_value);
            exit(EXIT_FAILURE);
        }
    } else {
        if(removexattr(file_path, ea_name)) {
            fprintf(stderr, "Fail to remove %s.\n", ea_name);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

/**
 * @brief 打印本程序使用或帮助信息
 * 
 */
void usage_err()
{
    fprintf(stderr, "Usage: setfattr -n USEREANAME [-V VALUE] file.\n");
    fprintf(stderr, "       setfattr -x USEREANAME file.\n");
    
    exit(EXIT_FAILURE);
}