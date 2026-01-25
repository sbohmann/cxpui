#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <execinfo.h>

constexpr int BACKTRACE_BUFFER_SIZE = 128;

static _Noreturn void print_backtrace_and_exit(void)
{
    void *buffer[BACKTRACE_BUFFER_SIZE];
    int backtrace_length = backtrace(buffer, BACKTRACE_BUFFER_SIZE);
    fprintf(stderr, "Backtrace:\n");
    backtrace_symbols_fd(buffer, backtrace_length, fileno(stderr));
    exit(EXIT_FAILURE);
}

_Noreturn void fail()
{
    fprintf(stderr, "Program execution failed.\n");
    print_backtrace_and_exit();
}

_Noreturn void fail_with_message(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Program execution failed: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    print_backtrace_and_exit();
}

_Noreturn void fail_with_errno()
{
    fprintf(stderr, "Program execution failed. Errno: %d - %s\n", errno, strerror(errno));
    print_backtrace_and_exit();
}

int main()
{
    fail_with_message("The numbers are %d and %d", 5, 7);
}
