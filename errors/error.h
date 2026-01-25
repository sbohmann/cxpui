#ifndef CXPUI_ERROR_H
#define CXPUI_ERROR_H

_Noreturn void fail();
_Noreturn void fail_with_message(const char *format, ...);
_Noreturn void fail_with_errno();

#endif
