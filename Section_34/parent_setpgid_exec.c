/**
 * @file parent_setpgid_exec.c
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
    int child_id;
    int s;

    switch(child_id = fork()) {
    case -1:
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    
    case 0: /* child process */
        // printf("After: child process %ld; process group %ld\n", 
        //     (long)getpid(), (long)getpgrp());
        // /* wait parent process change child process pgrp */
        // sleep(5);
        // printf("After: child process %ld; process group %ld\n", 
        //     (long)getpid(), (long)getpgrp());
        execl("./exec_bin", "exec_bin", (char*) NULL);
        printf("Servant exit\n");
        break;
    default:
        sleep(1);
        s = setpgid(child_id, 0);
        if (s == -1) {
            perror("setpgid");
        }
        printf("Main exit\n");
    }
    exit(EXIT_SUCCESS);
}