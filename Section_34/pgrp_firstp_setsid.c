/**
 * @file pgrp_firstp_setsid.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int s;
    s = setsid();
    if (s == -1) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}