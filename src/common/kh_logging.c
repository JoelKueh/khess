
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "kh_logging.h"

#define STRERROR_BUFSIZE 128

FILE *log_file;
pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;

kh_errno_t kh_mkerr(kh_error_t *err,  kh_errno_t kh_errno, char *fmt, ...)
{
    va_list args;
    err->num = kh_errno;
    va_start(args, fmt);
    vsnprintf(err->desc, KH_ERROR_STRLEN, fmt, args);
    va_end(args);
    return kh_errno;
}

kh_errno_t kh_strerror(kh_error_t *err, kh_errno_t kh_errno, char *prefix, int err_code)
{
    char buf[STRERROR_BUFSIZE];
    strerror_r(err_code, buf, STRERROR_BUFSIZE);
    snprintf(err->desc, KH_ERROR_STRLEN, "%s: %s\n", prefix, buf);
    return kh_errno;
}

void kh_init_log(FILE *f)
{
    log_file = f;
}

void kh_write_log(char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;

    if (log_file == NULL)
        return;
    t = time(NULL);
    tm = *localtime(&t);

    va_start(args, fmt);
    pthread_mutex_lock(&log_mtx);
    fprintf(log_file, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(log_file, fmt, args);
    pthread_mutex_unlock(&log_mtx);
    va_end(args);
}

void kh_vwrite_log(char *fmt, va_list args)
{
    time_t t;
    struct tm tm;

    if (log_file == NULL)
        return;
    t = time(NULL);
    tm = *localtime(&t);

    pthread_mutex_lock(&log_mtx);
    fprintf(log_file, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(log_file, fmt, args);
    pthread_mutex_unlock(&log_mtx);
}

void kh_perror(char *prefix, int err_code)
{
    time_t t;
    struct tm tm;
    char buf[STRERROR_BUFSIZE];

    if (log_file == NULL)
        return;
    t = time(NULL);
    tm = *localtime(&t);

    pthread_mutex_lock(&log_mtx);
    fprintf(log_file, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    strerror_r(err_code, buf, STRERROR_BUFSIZE);
    fprintf(log_file, "%s: %s\n", prefix, buf);
    pthread_mutex_unlock(&log_mtx);
}
