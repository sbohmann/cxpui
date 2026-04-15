/* Generated Code */

#ifndef VIEW_LIST
#define VIEW_LIST

#include <stddef.h>
#include "cxpui.h"

struct ViewList;

struct ViewList * ViewList_create(void);

void ViewList_delete(struct ViewList *self);

size_t ViewList_size(struct ViewList const *self);

struct ModuleState * ViewList_get(struct ViewList const *self, size_t index);

void ViewList_add(struct ViewList *self, struct ModuleState *newElement);

void ViewList_print(struct ViewList *self);

#endif
