#pragma once

#include <stdint.h>
#include <stdbool.h>

struct GraphicsContext {
    double width;
    double height;
};

// These numeric values are just guards against random / zero initialization
enum ViewType {
    CompositeView = 0x557a5bd49e199c91,
    CustomView = 0x5fba0d31c9ca5308,
    NativeView = 0xc739750fb1c91eff,
    Window = 0x1dce1c7d57b5290f
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

struct Window * start(void);

void Application_run(void);

struct View * View_create(void);

struct Window * View_as_window(struct View *view);

struct CustomView {
    struct View base;
    void (*paint)(struct GraphicsContext *context, struct Handle *handle);
};

struct CustomView * CustomView_create(void);

struct NativeView {
    struct View base;
    void *native_instance;
};

struct NativeView * NativeView_create(void *native_instance);

struct Window {
    struct View base;
    void *native_instance;
    struct View *mainView;
};

struct Window * Window_create(void *native_instance);

void Window_set_main_view(struct Window *window, struct View *view);

struct Handle {
    struct View *instance;
    void *holder_context;
    bool owned; // Owned by another view as part of the view graph
    bool held; // Held from user space, i.e., not released
};
