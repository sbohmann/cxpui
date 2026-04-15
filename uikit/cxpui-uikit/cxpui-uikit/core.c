#include "core.h"

#include <stdlib.h>

#include "errors/error.h"

void * allocate(size_t size) {
    void *result = malloc(size);
    if (result == NULL) {
        fail_with_message_and_errno("Unable to allocate %zu bytes of memory", size);
    }
    return result;
}
