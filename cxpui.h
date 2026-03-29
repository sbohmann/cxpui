#pragma once

#include <stdint.h>
#include <stdbool.h>

struct GraphicsContext {
    double width;
    double height;
};

enum ViewType {
    View,
    CustomView
};

struct View {
    enum ViewType type;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    struct Handle *handle;
};

struct CustomView {
    struct View control;
    void (*paint)(struct GraphicsContext *context, struct Handle *handle);
};

struct Handle {
    struct View *instance;
    void *holder_context;
    bool owned; // Owned by another view as part of the view graph
    bool held; // Held from user space, i.e., not released
};
