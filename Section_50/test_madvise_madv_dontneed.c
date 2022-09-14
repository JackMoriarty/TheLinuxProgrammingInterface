/**
 * @file test_madvise_madv_dontneed.c
 * @author Chen Bangduo (chenbangduo@qq.com)
 * @brief 测试advise函数的MADV_DONTNEED在MAP_PRIVATE映射上的操作
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

int main(int argc, char *argv[])
{
    void *mmap_addr;

    /* 匿名私有映射 */
    mmap_addr = mmap(NULL, 64, PROT_READ | PROT_WRITE, MAP_ANONYMOUS| MAP_PRIVATE, -1, 0);
    if (mmap_addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("0x%x\n", *(int *)mmap_addr);
    *(int *)mmap_addr = 0x123456;
    printf("0x%x\n", *(int *)mmap_addr);

    /* 调用madvise */
    if (madvise(mmap_addr, 64, MADV_DONTNEED) == -1) {
        perror("madvise");
        exit(EXIT_FAILURE);
    }

    printf("0x%x\n", *(int *)mmap_addr);

    exit(EXIT_SUCCESS);
}