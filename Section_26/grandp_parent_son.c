/**
 * @file grandp_parent_son.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 孙进程在父进程变为僵尸进程后何时被 1 号进程收养, 在祖父进程调用wait之前还是之后
 * @version 0.1
 * @date 2019-04-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    switch (fork())
    {
    case -1:
        perror("grandp fork");
        exit(EXIT_FAILURE);
    
    case 0:
        switch (fork())
        {
        case -1:
            perror("p fork");
            _exit(EXIT_FAILURE);   
        
        case 0:
            printf("son waiting\n");
            sleep(1);
            printf("pid before wait: %ld\n", (long)getppid());
            sleep(5);
            printf("pid after wait: %ld\n", (long)getppid());
            _exit(EXIT_SUCCESS);

        default:
            printf("P exit\n");
            _exit(EXIT_SUCCESS);
        }
    
    default:
        sleep(5);
        printf("pp before wait\n");
        wait(NULL);
        sleep(5);
        printf("pp exit\n");
        exit(EXIT_SUCCESS);
    }
}