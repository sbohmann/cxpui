#include "cxpui.h"

#include "core.h"
#include "view_list.h"
#include "errors/error.h"

const char* ViewType_to_string(enum ViewType type) {
    switch (type) {
    case CompositeView:
        return "CompositeView";
    case CustomView:
        return "CustomView";
    case NativeView:
        return "NativeView";
    case Window:
        return "Window";
    default:
        return "Unknown";
    }
}

struct Window* View_as_window(struct View* view) {
    if (view->type != Window) {
        fail_with_message("Failed to cast view to window. Actual type: %s for numeric value %x",
                          ViewType_to_string(view->type),
                          view->type);
    }
    return (struct Window*)view;
}

struct CompositeView* CompositeView_create(void) {
    struct CompositeView* result = allocate(sizeof(struct CustomView));
    *result = (struct CompositeView){0};
    result->base.type = CompositeView;
    result->sub_views = ViewList_create();
    return result;
}

void CompositeView_paint(
    struct CompositeView* self,
    void* context,
    void (*native_paint_view)(void* context, struct View* view)) {
    size_t size = ViewList_size(self->sub_views);
    for (size_t i = 0; i < size; i++) {
        struct View* view = ViewList_get(self->sub_views, i);
        native_paint_view(context, view);
    }
}

struct CustomView* CustomView_create() {
    struct CustomView* result = allocate(sizeof(struct CustomView));
    *result = (struct CustomView){0};
    result->base.type = CustomView;
    return result;
}
