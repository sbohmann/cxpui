#include "cxpui.h"

void draw_demo_view(struct GraphicsContext context);

int main(void) {
    struct Window *mainWindow = start();
    struct CustomView *demoView = CustomView_create();
    demoView->paint = &draw_demo_view;
    Window_set_main_view(mainWindow, (struct View *) demoView);
    Application_run();
}

int get_num_steps(struct GraphicsContext context) {
    int num_x_steps = (int) (context.width / 10);
    int num_y_steps = (int) (context.height / 10);
    return (num_x_steps + num_y_steps) / 2;
}

void draw_demo_view(struct GraphicsContext context) {
    int num_steps = get_num_steps(context);
    for (int index = 0; index <= num_steps; ++index) {
        double x = index * context.width / num_steps;
        double y = index * context.height / num_steps;
        line(context, x, 0, context.width, y);
        line(context, x, context.height, context.width, context.height - y);
        line(context, context.width - x, 0, 0, y);
        line(context, context.width - x, context.height, 0, context.height - y);
    }
    rect(context, context.width / 3.0, context.height / 3.0, context.width / 3.0, context.height / 3.0);
}
