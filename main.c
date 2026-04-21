#include "cxpui.h"
#include "view_list.h"

void draw_demo_view(struct GraphicsContext context);

int main(void) {
    struct Window *mainWindow = start();
    struct CompositeView *mainView = CompositeView_create();
    mainView->base.width = 800;
    mainView->base.height = 600;
    struct CustomView *demoView = CustomView_create();
    demoView->base.width = 800;
    demoView->base.height = 600;
    demoView->paint = &draw_demo_view;
    ViewList_add(mainView->sub_views, (struct View *) demoView);
    Window_set_main_view(mainWindow, (struct View *) mainView);
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
