#include <gtk/gtk.h>
#include <gsk/gsk.h>
#include "cxpui.h"

// Global Snapshot for Drawing Functions
GtkSnapshot *global_snapshot;

struct Fuck{int a, double b};

// Drawing Utilities
void line(double x1, double y1, double x2, double y2) {
    double thickness = 2.0;

    // Compute the rectangle representing the line
    double dx = x2 - x1;
    double dy = y2 - y1;
    double length = sqrt(dx * dx + dy * dy);

    graphene_matrix_t matrix;
    graphene_matrix_init_identity(&matrix);
    graphene_point3d_t start_position = {
        .x = (float) x1,
        .y = (float) y1,
        .z = 0};
    graphene_matrix_translate(&matrix, &start_position);
    graphene_vec3_t rotation_axis = {};
    graphene_vec3_init(&rotation_axis, 0, 0, 1);
    graphene_matrix_rotate(&matrix, (float) atan2(dy, dx), &rotation_axis);
    graphene_matrix_scale(&matrix, (float) length, (float) thickness, 1);

    GdkRGBA color = {0.0f, 0.0f, 1.0f, 1.0f}; // Blue

    graphene_rect_t rect = GRAPHENE_RECT_INIT(0, -thickness / 2, 1, 1); // Draw thin rectangle
    GskRenderNode *node = gsk_transform_node_new(
        gsk_color_node_new(&color, &rect), &matrix);
    gtk_snapshot_append_node(global_snapshot, node);
    g_object_unref(node);

}

void rect(double x, double y, double w, double h) {
    graphene_rect_t rect = GRAPHENE_RECT_INIT(x, y, w, h);
    GdkRGBA color = {1.0f, 0.0f, 0.0f, 1.0f}; // Red

    GskRenderNode *node = gsk_color_node_new(&color, &rect);
    gtk_snapshot_append_node(global_snapshot, node);
    g_object_unref(node);
}

void draw(struct Context context);

typedef struct {
    GObject parent_instance;
    int width;
    int height;
} CustomPaintable;

typedef struct {
    GObjectClass parent_class;
} CustomPaintableClass;

// Define Snapshot Behavior
static void custom_paintable_snapshot(GdkPaintable *paintable,
                                      GtkSnapshot *snapshot,
                                      double width,
                                      double height) {
    struct Context context = {width, height};

    // Set global snapshot for use in helper functions
    global_snapshot = snapshot;

    // Call drawing logic
    draw(context);

    // Reset global snapshot
    global_snapshot = nullptr;
}

// Define Intrinsic Paintable Size
static int custom_paintable_get_intrinsic_width(GdkPaintable *paintable) {
    return 400; // Default width
}

static int custom_paintable_get_intrinsic_height(GdkPaintable *paintable) {
    return 300; // Default height
}

// Initialize GdkPaintable Interface
static void custom_paintable_interface_init(GdkPaintableInterface *iface) {
    iface->snapshot = custom_paintable_snapshot;
    iface->get_intrinsic_width = custom_paintable_get_intrinsic_width;
    iface->get_intrinsic_height = custom_paintable_get_intrinsic_height;
}

// Custom Paintable Class Initialization
static void custom_paintable_class_init(CustomPaintableClass *klass) {
    // No additional initialization needed
}

// Custom Paintable Instance Initialization
static void custom_paintable_init(CustomPaintable *self) {
    self->width = 400; // Initialize default width
    self->height = 300; // Initialize default height
}

// Public Constructor for CustomPaintable
CustomPaintable *custom_paintable_new() {
    return g_object_new(custom_paintable_get_type(), NULL);
}

G_DEFINE_TYPE_WITH_CODE(
    CustomPaintable,
    custom_paintable,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(
        GDK_TYPE_PAINTABLE,
        custom_paintable_interface_init))

// GTK Application Activation
static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create a new application window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "cxpui gtk4 Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a GtkWidget and attach a custom GdkPaintable
    GtkWidget *paintable_widget = gtk_widget_paintable_new(GTK_TYPE_WIDGET);
    gtk_widget_set_size_request(paintable_widget, 400, 300);

    // Assign the custom paintable
    CustomPaintable *paintable = custom_paintable_new();
    gtk_widget_set_paintable(GTK_WIDGET(paintable_widget), GDK_PAINTABLE(paintable));
    g_object_unref(paintable);

    // Add the widget to the application window
    gtk_window_set_child(GTK_WINDOW(window), paintable_widget);

    // Show the window
    gtk_window_present(GTK_WINDOW(window));
}

// Main Entry Point
void start() {
    // Create a new application
    GtkApplication *app = gtk_application_new("at.yeoman.cxpui.gtk4", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the "activate" event
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), 0, NULL);

    // Clean up and exit
    g_object_unref(app);
    exit(status);
}
