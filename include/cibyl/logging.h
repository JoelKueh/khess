
#ifndef CIBYL_LOGGING_H
#define CIBYL_LOGGING_H

#include <stdarg.h>
#include <stdio.h>

/**
 * @breif Initializes the log file.
 * @param fname The nabe of the file.
 * @return An error code describing any errors encountered.
 */
void cibyl_init_log(FILE *f);

/**
 * @breif Writes a message to a log file.
 * @param fmt A printf compatible format string.
 * @param ... Args for the format string.
 */
void cibyl_write_log(char *fmt, ...);

/**
 * @breif Writes a message to a log file.
 * @param fmt A printf compatible format string.
 * @param args Args for the format string.
 */
void cibyl_vwrite_log(char *fmt, va_list args);

/**
 * @breif Writes an error to a log file.
 * @param prefix The prefix (will be followed by ": ").
 * @param err_code The error code (passed to the OS equivalent of strerror).
 */
void cibyl_perror(char *prefix, int err_code);

#endif /* CIBYL_LOGGING_H */
