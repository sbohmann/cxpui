#include "cxpui.h"

void start(void);

int main(void) {
    start();
}

void line(double x1, double y1, double x2, double y2);

int get_num_steps(struct Context context) {
    int num_x_steps = (int) (context.width / 10);
    int num_y_steps = (int) (context.width / 10);
    return num_x_steps < num_y_steps ? num_x_steps : num_y_steps;
}

void draw(struct Context context) {
    int num_steps = get_num_steps(context);
    for (int index = 0; index <= num_steps; ++index) {
        double x = index * context.width / num_steps;
        double y = index * context.height / num_steps;
        line(x, 0, context.width, y);
        line(x, context.height, context.width, context.height - y);
        line(context.width - x, 0, 0, y);
        line(context.width - x, context.height, 0, context.height - y);
    }
}
