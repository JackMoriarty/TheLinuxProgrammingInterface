/**
 * @file mem_segments.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 程序变量分配各段
 * @version 0.1
 * @date 2018-12-30
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>

char globBuf[65536];
int primes[4] = {2, 3, 5, 7};

static int square(int x)
{
    int result;

    result = x*x;
    return result;
}

static void doCalc(int val)
{
    printf("The square of %d is %d\n", val, square(val));

    if (val < 1000) {
        int t;
        t = val * val * val;
        printf("The cube of %d is %d\n", val, t);
    }
}

int main(int argc, char *argv[])
{
    static int key = 9973;
    static char mbuf[10240000];
    char *p;

    p = malloc(1024);

    doCalc(key);
    
    exit(EXIT_SUCCESS);
}