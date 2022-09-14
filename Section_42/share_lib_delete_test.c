/**
 * @file share_lib_delete_test.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 测试库中符号仍在使用的时候关闭库是否导致库的删除
 * @version 0.1
 * @date 2019-06-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    void *lib_handler1, *lib_handler2;
    void (*func)();
    char *err;
    int s;

    /* 加载动态库 */
    lib_handler1 = dlopen("./libmy.so", RTLD_LAZY|RTLD_GLOBAL);
    if (lib_handler1 == NULL) {
        perror(dlerror());
        exit(EXIT_FAILURE);
    }
#ifdef USE_LIB
    lib_handler2 = dlopen("./libtest.so", RTLD_LAZY);
    if (lib_handler2 == NULL) {
        perror(dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();
    func = dlsym(lib_handler2, "hh_printf");
    if (dlerror() != NULL) {
        fprintf(stderr, "Load failed\n");
        exit(EXIT_FAILURE);
    }
#endif

    /* 卸载库 */
    s = dlclose(lib_handler1);
    if (s == -1) {
        perror(dlerror());
        exit(EXIT_FAILURE);
    }
#ifdef USE_LIB
    (*func)();
#endif

    /* 检测库是否被卸载 */
    lib_handler1 = dlopen("./libmy.so", RTLD_NOLOAD|RTLD_LAZY|RTLD_GLOBAL);
    if (lib_handler1 == NULL) {
        printf("uninstall\n");
    } else {
        printf("install\n");
    }
    exit(EXIT_SUCCESS);
}