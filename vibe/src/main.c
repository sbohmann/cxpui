#include <stdio.h>

struct XPUIGC;

struct XPUIRectangle
{
    double x, y, width, height;
};

void XPUIGC_drawLine(struct XPUIGC *gc, int x1, int y1, int x2, int y2);

struct XPUIRectangle XPUIGC_bounds(struct XPUIGC *gc);

struct XPUICustomControl
{
    void (*draw)(struct XPUIGC *gc);
};

struct ExampleControl
{
    struct XPUICustomControl base;
    int32_t value;
};

void ExampleControl_draw(struct ExampleControl *self, struct XPUIGC *gc)
{
    struct XPUIRectangle bounds = XPUIGC_bounds(gc);
    XPUIGC_drawLine(gc, bounds.x, bounds.y, bounds.width / 2, bounds.height / 2);
    XPUIGC_drawLine(gc, bounds.x, bounds.y, self->value, self->value);
}

int main()
{
    printf("Hi\n");
}
