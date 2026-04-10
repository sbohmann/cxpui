#ifndef CXPUI_ERROR_H
#define CXPUI_ERROR_H

_Noreturn void fail(void);
_Noreturn void fail_with_message(const char *format, ...);
_Noreturn void fail_with_errno();

struct error
{
    const char *error_message;
    void *backtrace;
};

struct error *error(const char *format, ...);

void error_delete(const struct error *error);

#endif
