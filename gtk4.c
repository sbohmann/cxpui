#include <gtk/gtk.h>
#include "cxpui.h"

cairo_t *global_cr = nullptr;

void line(double x1, double y1, double x2, double y2) {
    if (!global_cr) return;
    cairo_set_line_width(global_cr, 2.0);  // Set the width of the line
    cairo_set_source_rgb(global_cr, 0, 0, 1);  // Set color to blue (RGB: 0, 0, 1)
    cairo_move_to(global_cr, x1, y1);  // Move to the starting point of the line
    cairo_line_to(global_cr, x2, y2);  // Draw a line to the end point
    cairo_stroke(global_cr);  // Render the line to the context
}

void rect(double x, double y, double w, double h) {
    if (!global_cr) return;
    cairo_set_source_rgb(global_cr, 1, 0, 0);  // Set color to red (RGB: 1, 0, 0)
    cairo_rectangle(global_cr, x, y, w, h);  // Define the rectangle
    cairo_fill(global_cr);  // Fill the rectangle with the current color
}

void draw(struct Context context);

static void draw_callback(GtkDrawingArea *drawing_area,
         cairo_t        *cr,
         int             width,
         int             height,
         gpointer        data) {
    struct Context context = {width, height};

    // Replace AppKit-specific logic with GTK rendering
    global_cr = cr;  // Equivalent to CGContext
    draw(context);
    global_cr = nullptr;
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create a new application window
    GtkWidget *window = gtk_application_window_new(app); // Attach the window to the application
    gtk_window_set_title(GTK_WINDOW(window), "cxpui gtk4 Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a GtkDrawingArea for custom rendering
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Present the window (show it)
    gtk_widget_set_visible(drawing_area, true);
    gtk_window_present(GTK_WINDOW(window));

    // Connect the "draw" signal to the drawing callback
    // g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_callback), NULL);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_callback, NULL, nullptr);

    gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

void start() {
    // Create a new GtkApplication
    GtkApplication *app = gtk_application_new("at.yeoman.cxpui.gtk4", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the "activate" signal to our activation function
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), 0, nullptr);

    // Clean up
    g_object_unref(app);

    // Exit the application (status is returned from the g_application_run loop)
    exit(status);
}
