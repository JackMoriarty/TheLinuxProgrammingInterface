#ifndef _BECOME_DAEMON_H_
#define _BECOME_DAEMON_H_

/* become_daemon函数标志掩码 */
#define BD_NO_CHDIR             01  /* 不使用chdir */
#define BD_NO_CLOSE_FILES       02  /* 不关闭所有打开的文件 */
#define BD_NO_REOPEN_STD_FDS    04  /* 不重新打开stdin, stdout, stderr到/dev/null */
#define BD_NO_UMASKD            010     /* 不清除进程umask */
#define BD_MAX_CLOSE            8192    /* 关闭文件描述符最大值, 如果相应数量限制不可用的话 */

int become_daemon(int flags);

#endif