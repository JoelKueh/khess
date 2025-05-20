
#ifndef KHESS_ERROR_H
#define KHESS_ERROR_H

#define KH_ERROR_STRLEN 128

#include <stdarg.h>
#include <stdio.h>

/**
 * @breif Error codes for the.
 */
typedef enum {
    KH_ENOMEM = -2,     /**< Non-recoverable error for malloc failures. */
    KH_EABORT = -1,     /**< Generic non-recoverable error. */
    KH_EOK = 0,         /**< Default return value. */
    KH_EINVAL,          /**< Recoverable invalid formatting error. */
    KH_EILLEGAL,        /**< Recoverable illegal move specification error. */
} kh_errno_t;

/**
 * @breif Defines an error type for errors that will be returned by the library.
 */
typedef struct {
    kh_errno_t num;                 /**< The error code for the error. */
    char desc[KH_ERROR_STRLEN];     /**< A string description of the error. */
} kh_error_t;

/**
 * @breif Function that populates an error struct with error information.
 *
 * Thread safe.
 *
 * @param err The error struct to populate.
 * @param cb_errno The error number.
 * @param fmt A printf compliant format string.
 * @param ... All remaining arguments passed into the format string.
 * @return Passthrough for kh_errno.
 */
kh_errno_t kh_mkerr(kh_error_t *err,  kh_errno_t kh_errno, char *fmt, ...);

/**
 * @breif Provides a platform independent interface similar to strerror.
 *
 * Thread safe.
 *
 * @param err The error struct to populate.
 * @param kh_errno The error number to return.
 * @param prefix The prefix (will be followed by ": ").
 * @param err_code The error code (passed to the OS equivalent of strerror).
 * @return Passthrough for kh_errno.
 */
kh_errno_t kh_strerror(kh_error_t *err, kh_errno_t kh_errno, char *prefix, int err_code);

/**
 * @breif Initializes the log file.
 * @param fname The nabe of the file.
 * @return An error code describing any errors encountered.
 */
void kh_init_log(FILE *f);

/**
 * @breif Writes a message to a log file.
 * @param fmt A printf compatible format string.
 * @param ... Args for the format string.
 */
void kh_write_log(char *fmt, ...);

/**
 * @breif Writes a message to a log file.
 * @param fmt A printf compatible format string.
 * @param args Args for the format string.
 */
void kh_vwrite_log(char *fmt, va_list args);

/**
 * @breif Writes an error to a log file.
 * @param prefix The prefix (will be followed by ": ").
 * @param err_code The error code (passed to the OS equivalent of strerror).
 */
void kh_perror(char *prefix, int err_code);

#endif /* KHESS_ERROR_H */
