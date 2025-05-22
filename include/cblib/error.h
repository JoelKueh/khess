
#include "types.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define STRERROR_BUFSIZE 128

#ifndef CB_ERROR_H
#define CB_ERROR_H

static inline cb_errno_t cb_mkerr(cb_error_t *err,  cb_errno_t cb_errno, char *fmt, ...)
{
    va_list args;
    err->num = cb_errno;
    va_start(args, fmt);
    vsnprintf(err->desc, CB_ERROR_STRLEN, fmt, args);
    va_end(args);
    return cb_errno;
}

static inline cb_errno_t cb_strerror(cb_error_t *err, cb_errno_t cb_errno,
        char *prefix, int err_code)
{
    char buf[STRERROR_BUFSIZE];
#ifdef WIN32
    strerror_s(buf, err_code, STRERROR_BUFSIZE);
#else
    strerror_r(err_code, buf, STRERROR_BUFSIZE);
#endif
    snprintf(err->desc, CB_ERROR_STRLEN, "%s: %s\n", prefix, buf);
    return cb_errno;
}

#endif /* CB_ERROR_H */
