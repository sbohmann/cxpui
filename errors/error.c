#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>

#define BACKTRACE_BUFFER_SIZE 1024

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

// struct error *error(const char *format, ...)
// {
//     struct error *result = malloc(sizeof(struct error));
//     if (result == NULL)
//     {
//         fail_with_message("Failed to allocate struct error of size ", sizeof(struct error));
//     }
//     *result = (struct error) {
//         .error_message =
//     };
// }
//
// void error_delete(const struct error *error)
// {
//
// }
//
// int main()
// {
//     fail_with_message("The numbers are %d and %d", 5, 7);
// }
