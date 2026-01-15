#include <xpui/gc.h>
#include <xpui/events.h>

struct Example {
    XPUICustomControl base;
    int32 mode;
};

void Example_draw_control(struct Example *self, XPUIGC gc) {
    ...
}

void Example_handleMouseDown(struct Example *self, MouseDownEvent event) {
    ...
}
