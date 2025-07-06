#include <gtk/gtk.h>
#include <gsk/gsk.h>
#include "cxpui.h"

// Drawing utilities
void line(GtkSnapshot *snapshot, double x1, double y1, double x2, double y2) {
    graphene_point_t start = {x1, y1};
    graphene_point_t end = {x2, y2};
    GdkRGBA color = {0.0, 0.0, 1.0, 1.0}; // Blue

    GskRenderNode *node = gsk_line_node_new(&start, &end, &color, 2.0);
    gtk_snapshot_append_node(snapshot, node);
    g_object_unref(node);
}

void rect(GtkSnapshot *snapshot, double x, double y, double w, double h) {
    graphene_rect_t rect = GRAPHENE_RECT_INIT(x, y, w, h);
    GdkRGBA color = {1.0, 0.0, 0.0, 1.0}; // Red

    GskRenderNode *node = gsk_color_node_new(&color, &rect);
    gtk_snapshot_append_node(snapshot, node);
    g_object_unref(node);
}

void draw(struct Context context, GtkSnapshot *snapshot);

static void draw_callback(GtkDrawingArea *drawing_area,
                          GtkSnapshot *snapshot,
                          int width,
                          int height,
                          gpointer user_data) {
    struct Context context = {width, height};
    draw(context, snapshot);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "cxpui gtk4 Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create drawing area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Set draw function
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_callback, NULL, NULL);

    // Show window
    gtk_window_present(GTK_WINDOW(window));
}

void start() {
    GtkApplication *app = gtk_application_new("at.yeoman.cxpui.gtk4", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), 0, NULL);

    g_object_unref(app);
    exit(status);
}