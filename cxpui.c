#include "cxpui.h"

#include "errors/error.h"

const char * ViewType_to_string(enum ViewType type) {
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

struct Window * View_as_window(struct View *view) {
    if (view->type != Window) {
        fail_with_message("Failed to cast view to window. Actual type: %s for numeric value %x",
            ViewType_to_string(view->type),
            view->type);
    }
    return (struct Window *)view;
}
