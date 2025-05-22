
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <event2/event.h>
#include <event2/util.h>

#include "logging.h"

#define STRERROR_BUFSIZE 256

FILE *log_file;
pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;

static void libevent_log_cb(int severity, const char *msg)
{
    const char *s;

    /* Return if there is no log file. */
    if (log_file == NULL)
        return;

    switch (severity) {
        case _EVENT_LOG_DEBUG: s = "debug"; break;
        case _EVENT_LOG_MSG:   s = "msg";   break;
        case _EVENT_LOG_WARN:  s = "warn";  break;
        case _EVENT_LOG_ERR:   s = "error"; break;
        default:               s = "?";     break; /* never reached */
    }

    kh_write_log("[%s] %s\n", s, msg);
}

void kh_init_log(FILE *f)
{
    log_file = f;
    if (f != NULL) {
        event_set_log_callback(libevent_log_cb);
    } else {
        event_set_log_callback(NULL);
    }
}

void kh_vwrite_log(char *fmt, va_list args)
{
    time_t t;
    struct tm tm;

    /* Return if there is no log file. */
    if (log_file == NULL)
        return;
    t = time(NULL);
    tm = *localtime(&t);

    /* Get the current time. */
    t = time(NULL);
#ifdef WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    /* Log the message. */
    pthread_mutex_lock(&log_mtx);
    fprintf(log_file, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(log_file, fmt, args);
    pthread_mutex_unlock(&log_mtx);
}

void kh_write_log(char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;

    va_start(args, fmt);
    kh_vwrite_log(fmt, args);
    va_end(args);
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
