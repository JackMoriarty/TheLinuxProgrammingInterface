/**
 * @file longjmp.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 跳转到已经返回的函数上
 * @version 0.1
 * @date 2018-12-30
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

int func_b()
{
    longjmp(env, 1);
    printf("There is func_b\n");
    return 0;
}

int func_a()
{
    switch(setjmp(env)) {
        case 0:
            printf("Calling from main\n");
            func_b();
            break;
        case 1:
            printf("Go back from func_b\n");
            break;
    }
    printf("There is func_a\n");
    return 0;
}

int main()
{
    printf("There is main\n");

    func_a();
    func_b();

    return 0;
}