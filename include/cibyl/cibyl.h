
#ifndef CIBYL_H
#define CIBYL_H

#include "cb_lib.h"

typedef enum {
    /* Non-recoverable errors. Almost all errors are treated this way. */
    CIBYL_ENOMEM = -2,
    CIBYL_EABORT = -1,

    /* This is okay, default return value for any function that can err. */
    CIBYL_EOK = 0,
} cibyl_errno_t;

#endif /* CIBYL_H */
