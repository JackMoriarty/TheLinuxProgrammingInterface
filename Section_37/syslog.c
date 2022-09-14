/**
 * @file syslog.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 编写使用 syslog 的程序来将任意的消息写到系统日志中
 * @version 0.1
 * @date 2019-05-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

extern char *optarg;
extern int optind;

static void usageError(const char *prog_name)
{
    fprintf(stderr, "Usage: %s [-p] [-e] [-l level] \"message\"\n", prog_name);
    fprintf(stderr, "       -p  log PID\n");
    fprintf(stderr, "       -e  log to stderr also\n");
    fprintf(stderr, "       -l  level   (g=EMERG; a=ALERT; c=CRIT; e=ERR\n");
    fprintf(stderr, "                    w=WARNING; n=NOTICE; i=INFO; d=DEBUG");
    
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int level, options, opt;

    options = 0;
    level = LOG_INFO;

    while ((opt = getopt(argc, argv, "l:pe")) != -1) {
        switch (opt) {
        case 'l':
            switch (optarg[0]) {
            case 'g': level = LOG_EMERG;    break;
            case 'a': level = LOG_ALERT;    break;
            case 'c': level = LOG_CRIT;     break;
            case 'e': level = LOG_ERR;      break;
            case 'w': level = LOG_WARNING;  break;
            case 'n': level = LOG_NOTICE;   break;
            case 'i': level = LOG_INFO;     break;
            case 'd': level = LOG_DEBUG;    break;
            default: 
                fprintf(stderr, "Bad facility: %c\n", optarg[0]);
                exit(EXIT_FAILURE);
            }
            break;

        case 'p':
            options |= LOG_PID;
            break;
        
        case 'e':
            options |= LOG_PERROR;
            break;
        
        default:
            fprintf(stderr, "Bad option\n");
            usageError(argv[0]);
        }
    }

    if (argc != optind + 1)
        usageError(argv[0]);

    openlog(argv[0], options, LOG_USER);
    syslog(LOG_USER | level, "%s", argv[optind]);
    closelog();

    exit(EXIT_SUCCESS);
}