/* Generated Code */

#include "view_list.h"

#include <stdlib.h>
#include <string.h>

#include "errors/error.h"

struct ViewList {
    struct ModuleState ** data;
    size_t size;
    size_t capacity;
};

struct ViewList * ViewList_create(void) {
    struct ViewList *self = malloc(sizeof(struct ViewList));
    *self = (struct ViewList) {
        NULL,
        0,
        0
    };
    return self;
}

void ViewList_delete(struct ViewList *self) {
    free(self->data);
    free(self);
}

size_t ViewList_size(struct ViewList const *self) {
    return self->size;
}

struct ModuleState * ViewList_get(struct ViewList const *self, size_t index) {
    if (index >= self->size) {
        fail_with_message("Index out of bounds [%zu] at size %zu", index, self->size);
    }
    return self->data[index];
}

void ViewList_add(struct ViewList *self, struct ModuleState *newElement) {
    if (self->size < self->capacity) {
        self->data[self->size] = newElement;
        ++self->size;
    } else {
        const size_t new_capacity = self->capacity * 3 / 2 + 1;
        struct ModuleState ** new_data = malloc(sizeof(struct ModuleState *) * new_capacity);
        if (self->data != NULL) {
            memcpy(new_data, self->data, sizeof(struct ModuleState *) * self->size);
            free(self->data);
        }
        new_data[self->size] = newElement;
        ++self->size;
        self->data = new_data;
        self->capacity = new_capacity;
    }
}
