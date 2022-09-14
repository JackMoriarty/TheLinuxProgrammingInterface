/**
 * @file dynload.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief dynload 改进版
 * @version 0.1
 * @date 2019-06-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

int main(int argc, char *argv[])
{
    void *libHandle;
    void (*funcp)(void);
    const char *err;
    Dl_info di;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s lib-path func-name\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* 加载共享库 */
    libHandle = dlopen(argv[1], RTLD_LAZY);
    if (libHandle == NULL) {
        fprintf(stderr, "dlopen %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    /* 在库中所有相应符号 */
    (void)dlerror();
    funcp = dlsym(libHandle, argv[2]);
    err = dlerror();
    if (err != NULL) {
        fprintf(stderr, "dlsym: %s", err);
        exit(EXIT_FAILURE);
    }

    /* 如果返回的地址不为NULL, 则调用该函数 */
    if (funcp == NULL)
        printf("%s is NULL\n", argv[2]);
    else 
        (*funcp)();
    
    /* 查询相关地址信息 */
    if (dladdr(funcp, &di))
        printf("%s %ld %s %ld\n", 
            di.dli_fname, (long)di.dli_fbase, di.dli_sname, (long)di.dli_saddr);
    else
        printf("Find %ld failed\n", (long)funcp);
    
    dlclose(libHandle);
    exit(EXIT_SUCCESS);
}