/**
 * @file my_dirname_basename_mt.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 实现线程安全的dirname 和 basename
 * @version 0.1
 * @date 2019-04-29
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

static pthread_key_t dirname_key;
static pthread_key_t basename_key;
static pthread_once_t dirname_once = PTHREAD_ONCE_INIT;
static pthread_once_t basename_once = PTHREAD_ONCE_INIT;

/**
 * @brief 析构函数
 * 
 * @param buf 待释放的空间指针
 */
static void destructor(void *buf)
{
    free(buf);
}

/**
 * @brief 创建dirname的key
 * 
 */
static void create_dirname_key(void)
{
    int s;
    s = pthread_key_create(&dirname_key, destructor);
    if (s != 0) {
        fprintf(stderr, "create_dirname_key: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief 创建basename的key
 * 
 */
static void create_basename_key(void)
{
    int s;
    s = pthread_key_create(&basename_key, destructor);
    if (s != 0) {
        fprintf(stderr, "create_basename_key: %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief 线程安全的dirname
 * 
 * @param pathname 路径名
 * @return char* 路径
 */
char *my_dirname_mt(char *pathname)
{
    int s;
    char *buf;
    char *ptr;
    int strlength;
    int i;
    
    s = pthread_once(&dirname_once, create_dirname_key);
    if (s != 0) {
        fprintf(stderr, "dirname_once: %s\n", strerror(s));
        exit(EXIT_SUCCESS);
    }

    buf = pthread_getspecific(dirname_key);
    if (buf == NULL) {
        buf = (char *)malloc(sizeof(char) * PATH_MAX);
        if (buf == NULL) {
            fprintf(stderr, "malloc path\n");
            exit(EXIT_FAILURE);
        }

        s = pthread_setspecific(dirname_key, buf);
        if (s != 0) {
            fprintf(stderr, "dirname_setspec: %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    }

    /* 空字符串或空指针 */
    if (pathname == NULL || strlen(pathname) == 0) {
        strcpy(buf, ".");
        return buf;
    }
    /* pathname 仅由一个斜线构成 */
    if (*pathname == '/' && *(pathname + 1) == '\0') {
        strcpy(buf, "/");
        return buf;
    }

    /* 确定路径的边界 */
    strlength = strlen(pathname);
    ptr = pathname + strlength - 1;
    /* 忽略尾部的斜线字符 */
    while (ptr > pathname && *ptr == '/' && *(ptr - 1) == '/')
        ptr--;
    if (*ptr == '/' && ptr != pathname) ptr--;
    /* 向前搜索斜线符 */
    while (*ptr != '/' && ptr != pathname) 
        ptr--;
    
    /* ptr 之前的为路径 */
    /* 路径不含斜线符 */
    if (*ptr != '/') {
        strcpy(buf, ".");
        return buf;
    } else {
        /* pathname --> ptr 之间为路径 */
        char *tmp_ptr = pathname;
        i = 0;
        while (1) {
            while(*tmp_ptr == '/' && *(tmp_ptr + 1) == '/') tmp_ptr++;
            if (tmp_ptr == ptr)
                break;
            buf[i] = *tmp_ptr;
            tmp_ptr++;
            i++;
        }
        buf[i] = '\0';

        return buf;
    }


}

/**
 * @brief 线程安全的basename
 * 
 * @param pathname 路径名
 * @return char* 文件名
 */
char *my_basename_mt(char *pathname)
{
    int s;
    char *buf;
    char *ptr;
    int strlength;
    int i;
    
    s = pthread_once(&basename_once, create_basename_key);
    if (s != 0) {
        fprintf(stderr, "basename_once: %s\n", strerror(s));
        exit(EXIT_SUCCESS);
    }

    buf = pthread_getspecific(basename_key);
    if (buf == NULL) {
        buf = (char *)malloc(sizeof(char) * FILENAME_MAX);
        if (buf == NULL) {
            fprintf(stderr, "malloc filename\n");
            exit(EXIT_FAILURE);
        }

        s = pthread_setspecific(basename_key, buf);
        if (s != 0) {
            fprintf(stderr, "dirname_setspec: %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    }

    /* 空字符串或空指针 */
    if (pathname == NULL || strlen(pathname) == 0) {
        strcpy(buf, ".");
        return buf;
    }
    /* pathname 仅由一个斜线构成 */
    if (*pathname == '/' && *(pathname + 1) == '\0') {
        strcpy(buf, "/");
        return buf;
    }

    /* 确定路径的边界 */
    strlength = strlen(pathname);
    ptr = pathname + strlength - 1;
    /* 忽略尾部的斜线字符 */
    while (ptr > pathname && *ptr == '/' && *(ptr - 1) == '/')
        ptr--;
    if (*ptr == '/' && ptr != pathname) ptr--;
    /* 向前搜索斜线符 */
    while (*ptr != '/' && ptr != pathname) 
        ptr--;
    
    /* ptr 之后的为 basename */
    if(ptr != pathname) ptr++;
    i = 0;
    while(ptr[i] != '/' && ptr[i] != '\0') {
        buf[i] = ptr[i];
        i++;
    }
    buf[i] = '\0';

    return buf;
}


static void *thread_func(void *arg)
{
    char pathname[] = "//////bcd////efg////";
    char *dirname = my_dirname_mt(pathname);
    char *basename = my_basename_mt(pathname);

    printf("S:%s:%s\n", dirname, basename);
}

int main(int argc, char *argv[])
{
    char pathname[] = "//////etc////passwd////";
    // char *pathname = NULL;
    char *dirname = my_dirname_mt(pathname);
    char *basename = my_basename_mt(pathname);
    pthread_t t;

    pthread_create(&t, NULL, thread_func, NULL);
    pthread_join(t, NULL);

    printf("M:%s:%s\n", dirname, basename);

    exit(EXIT_SUCCESS);
}