/**
 * @file cap_sched_set.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 利用能力模型更改35-2程序
 * @version 0.1
 * @date 2019-06-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/capability.h>
#include <errno.h>

/**
 * @brief 提权
 * 
 * @return int 0 表示成功, -1 表示失败 
 */
static int raise_cap_sys_nice()
{
    cap_t caps;
    cap_value_t capList[1];
    caps = cap_get_proc();
    if (caps == NULL)
        return -1;
    
    capList[0] = CAP_SYS_NICE;
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, capList, CAP_SET) == -1) {
        cap_free(caps);
        return -1;
    }

    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    if (cap_free(caps) == -1)
        return -1;
    
    return 0;
}

int main(int argc, char *argv[])
{
    int j, pol;
    struct sched_param sp;

    if (argc < 3 || strchr("rfo", argv[1][0]) == NULL) {
        fprintf(stderr, "%s policy priority [pid...]\n"
                    "policy is 'r'(RR), 'f'(FIFO),"
#ifdef SCHED_BATCH
                    "'b'(BATCH),"
#endif
#ifdef SCHED_IDLE
                    "'i'(IDLE),"
#endif
                    "or 'o'(OTHER)\n"
                    , argv[0]);
        exit(EXIT_FAILURE);
    }

    pol = (argv[1][0] == 'r') ? SCHED_RR :
            (argv[1][0] == 'f') ? SCHED_FIFO :
#ifdef SCHED_BATCH
            (argv[1][0] == 'b') ? SCHED_BATCH :
#endif
#ifdef SCHED_IDLE
            (argv[1][0] == 'i') ? SCHED_IDLE :
#endif
            SCHED_OTHER;

    sp.sched_priority = atoi(argv[2]);

    /* 提升能力 */
    if (raise_cap_sys_nice() == -1) {
        fprintf(stderr, "raise failed %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (j = 3; j < argc; j++) {
        if (sched_setscheduler(atol(argv[j]), pol, &sp) == -1) {
            perror("sched_setscheduler");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}