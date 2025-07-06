#include <gtk/gtk.h>
#include <gsk/gsk.h>
#include "cxpui.h"

// Drawing utilities using GtkSnapshot
void line(GtkSnapshot *snapshot, double x1, double y1, double x2, double y2) {
    // Create a blue line between the points
    GskRenderNode *node = gsk_line_node_new(
        &(graphene_point_t){x1, y1},
        &(graphene_point_t){x2, y2},
        gdk_rgba_new(0, 0, 1, 1), // Color (Blue)
        2.0                        // Line width
    );
    gtk_snapshot_append_node(snapshot, node);
    g_object_unref(node);
}

void rect(GtkSnapshot *snapshot, double x, double y, double w, double h) {
    // Create a red rectangle
    GdkRGBA color = {1.0, 0.0, 0.0, 1.0}; // Color (Red)
    GskRenderNode *node = gsk_color_node_new(&color, &(graphene_rect_t){x, y, w, h});
    gtk_snapshot_append_node(snapshot, node);
    g_object_unref(node);
}

void draw(struct Context context, GtkSnapshot *snapshot);

static void draw_callback(GtkDrawingArea *drawing_area,
                          GtkSnapshot *snapshot,
                          int width,
                          int height,
                          gpointer data) {
    struct Context context = {width, height};
    draw(context, snapshot);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create a new application window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "cxpui gtk4 Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a GtkDrawingArea
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Connect the "draw" function
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_callback, NULL, NULL);

    // Show the window
    gtk_window_present(GTK_WINDOW(window));
}

void start() {
    // Create a new GtkApplication
    GtkApplication *app = gtk_application_new("at.yeoman.cxpui.gtk4", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the "activate" signal
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), 0, NULL);

    // Clean up
    g_object_unref(app);
    exit(status);
}