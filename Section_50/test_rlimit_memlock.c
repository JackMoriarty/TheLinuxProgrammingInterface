/**
 * @file test_rlimit_memlock.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-07-06
 * 
 * @copyright Copyright (c) 2019 Chen Bangduo. All Rights Reserved.
 * 
*/

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    long page_size;
    int page_num;
    int i;

    /* 参数检查 */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s page_num\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 获取页大小 */
    page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        perror("sysconf - PAGE_SIZE");
        exit(EXIT_FAILURE);
    }

    page_num = atoi(argv[1]);

    for (i = 0; i < page_num; i++) {
        printf("Mapping index [%d] ", i);
        fflush(stdout);
        if (mmap(NULL, page_size, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE | MAP_LOCKED, -1, 0) == MAP_FAILED){
            fprintf(stderr, "Failed\n");
        } else {
            printf("Success\n");
        }
    }

    exit(EXIT_SUCCESS);
}