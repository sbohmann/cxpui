# Error Handling in C

Traditionally, error handling in C works through special return values. Example:

```c
FILE *fopen(const char *path, const char *mode);
```

Here, `NULL` is returned if when the operation fails, and `errno` is set accordingly when a failure is indicated by the return value (its value must be used / saved in a var immediately, of course, because subsequent calls may change it).

Sometimes, this is fine.

## The Two Kinds of Errors

Errors can be expected or unexpected.

This is a distinction that can only be made when a function is called, and it entirely depends on context.

When Java was created, this idea was reflected by introducing *checked exceptions* for expected errors. Unfortunately, this moved the decision about whether an error was expected to the implementation of a method.

Only the calling site can decide, though, whether an error is actually expected. Opening a configuration file that is part of an executable is expected to succeed, so if it's missing, unexpected error. Opening a file from a user-provided path, on the other hand, should be expected to fail.

### Unexpected Errors

These are the vast majority of errors during program execution. Their sources are many, and in some cases they're impossible or hard to recover from even at a technical level - stack overflow, out of memory, &c.

Many of them are unexpected because they breach a basic assumptions, like configuration files that belong to the executable itself have not been deleted and the file system has not been corrupted.

Many of them originate from the practice of defensive programming and the ideal of failing fast. These are usually triggered in response to failed checks about the consistency and correctness of algorithms and data structures.

```c
if (result.size != 32) {
    fail_with_message("Logical error - result.size [%zu] != 32", result.size);
}
```

What is fail_with_message?

We'll build that further down.

## Expected Errors


