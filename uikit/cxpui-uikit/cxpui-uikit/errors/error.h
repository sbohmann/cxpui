#ifndef CXPUI_ERROR_H
#define CXPUI_ERROR_H

#include <stdnoreturn.h>

noreturn void fail(void);
noreturn void fail_with_message(const char *format, ...);
noreturn void fail_with_errno();
noreturn void fail_with_message_and_errno(const char *format, ...);

struct error
{
    const char *error_message;
    void *backtrace;
};

#endif
