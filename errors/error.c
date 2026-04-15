#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>

#define BACKTRACE_BUFFER_SIZE 1024

#define PRINT(format) \
    va_list arguments; \
    va_start(arguments, (format)); \
    vfprintf(stderr, (format), arguments); \
    va_end(arguments);

static void newline(void) {
    fputc('\n', stderr);
}

static _Noreturn void print_backtrace_and_exit(void)
{
    void *buffer[BACKTRACE_BUFFER_SIZE];
    int backtrace_length = backtrace(buffer, BACKTRACE_BUFFER_SIZE);
    fprintf(stderr, "Backtrace:\n");
    backtrace_symbols_fd(buffer, backtrace_length, STDERR_FILENO);
    exit(EXIT_FAILURE);
}

_Noreturn void fail(void)
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
    int errno_value = errno;
    fprintf(stderr, "Program execution failed. Errno: %d - %s\n", errno_value, strerror(errno_value));
    print_backtrace_and_exit();
}

_Noreturn void fail_with_message_and_errno(const char *format, ...) {
    fflush(stdout);
    fprintf(stderr, "Failed with errno: %d - %s\n", errno, strerror(errno));
    if (format) {
        fputs("Reason: ", stderr);
        PRINT(format)
        newline();
    } else {
        fprintf(stderr, "fail was called.");
    }
    fail();
}
