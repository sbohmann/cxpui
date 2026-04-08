#pragma once

#include <stdint.h>
#include <stdbool.h>

struct GraphicsContext {
    double width;
    double height;
};

enum ViewType {
    CompositeView,
    CustomView,
    NativeView
};

struct View {
    enum ViewType type;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    struct Handle *handle;
};

struct CompositeView {
    struct View base;
};

struct View * View_create();

struct CustomView {
    struct View base;
    void (*paint)(struct GraphicsContext *context, struct Handle *handle);
};

struct CustomView * CustomView_create();

struct NativeView {
    struct View base;
    void *native_instance;
};

struct NativeView * NativeView_create(void *native_instance);

struct Handle {
    struct View *instance;
    void *holder_context;
    bool owned; // Owned by another view as part of the view graph
    bool held; // Held from user space, i.e., not released
};
