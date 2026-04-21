/*
 * GTK4 backend for cxpui — GSK render nodes, no cairo.
 *
 * Lifecycle: start() creates the GtkApplication and a Window struct.
 * The actual GtkWindow + canvas widget are created in on_activate,
 * which fires once Application_run() calls g_application_run().
 * By that point main.c has already called Window_set_main_view(),
 * so the view tree is ready to paint.
 *
 * Drawing: We subclass GtkWidget ("CxpuiCanvas") and override its
 * snapshot vfunc. line() and rect() build GskRenderNodes and append
 * them to the GtkSnapshot stored in GraphicsContext.native_context.
 */

#include <gtk/gtk.h>
#include <math.h>
#include "cxpui.h"
#include "view_list.h"
#include "core.h"

/* ── Global state ────────────────────────────────────────────────── */

static struct Window *g_mainWindow = NULL;
static GtkApplication *g_app       = NULL;
static GtkWidget      *g_canvas    = NULL;

/* ── CxpuiCanvas widget ─────────────────────────────────────────── */

#define CXPUI_TYPE_CANVAS (cxpui_canvas_get_type())
G_DECLARE_FINAL_TYPE(CxpuiCanvas, cxpui_canvas, CXPUI, CANVAS, GtkWidget)

struct _CxpuiCanvas {
    GtkWidget parent_instance;
};

static void paint_view(GtkSnapshot *snapshot, struct View *view);

static void cxpui_canvas_snapshot(GtkWidget *widget, GtkSnapshot *snapshot) {
    if (!g_mainWindow || !g_mainWindow->mainView)
        return;

    int width  = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);

    struct View *root = g_mainWindow->mainView;
    root->width  = width;
    root->height = height;

    /* Propagate size to immediate children of a CompositeView.
     * Placeholder for a real layout system. */
    if (root->type == CompositeView) {
        struct CompositeView *comp = (struct CompositeView *)root;
        size_t n = ViewList_size(comp->sub_views);
        for (size_t i = 0; i < n; i++) {
            struct View *child = ViewList_get(comp->sub_views, i);
            child->width  = width;
            child->height = height;
        }
    }

    paint_view(snapshot, root);
}

static void cxpui_canvas_class_init(CxpuiCanvasClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->snapshot = cxpui_canvas_snapshot;
}

static void cxpui_canvas_init(CxpuiCanvas *self) {
    (void)self;
}

G_DEFINE_TYPE(CxpuiCanvas, cxpui_canvas, GTK_TYPE_WIDGET)

static GtkWidget *cxpui_canvas_new(void) {
    return g_object_new(CXPUI_TYPE_CANVAS, NULL);
}

/* ── Drawing primitives (GSK) ────────────────────────────────────── */

void line(struct GraphicsContext gc, double x1, double y1, double x2, double y2) {
    GtkSnapshot *snapshot = (GtkSnapshot *)gc.native_context;
    if (!snapshot) return;

    double dx = x2 - x1;
    double dy = y2 - y1;
    double length = sqrt(dx * dx + dy * dy);
    if (length < 0.001) return;

    double angle_deg = atan2(dy, dx) * (180.0 / G_PI);
    float thickness = 2.0f;

    /*
     * Strategy: a 1x1 color node, scaled to (length x thickness),
     * rotated, and translated to (x1, y1).
     *
     * The unit rect is at (0, -0.5, 1, 1) so that after scaling by
     * (length, thickness) the line is centered on the axis:
     *   y: -0.5 * thickness  to  +0.5 * thickness
     *   x: 0                 to  length
     */
    graphene_rect_t unit_rect = GRAPHENE_RECT_INIT(0, -0.5f, 1, 1);
    GdkRGBA color = {0.0f, 0.0f, 1.0f, 1.0f}; /* blue */

    /* Transform order (applied innermost-first):
     *   1. scale to line dimensions
     *   2. rotate to correct angle
     *   3. translate to start point
     * GskTransform builds left-to-right = outermost-first, so: */
    GskTransform *xform = NULL;
    xform = gsk_transform_translate(xform,
                &GRAPHENE_POINT_INIT((float)x1, (float)y1));
    xform = gsk_transform_rotate(xform, (float)angle_deg);
    xform = gsk_transform_scale(xform, (float)length, thickness);

    GskRenderNode *color_node = gsk_color_node_new(&color, &unit_rect);
    GskRenderNode *xform_node = gsk_transform_node_new(color_node, xform);

    gtk_snapshot_append_node(snapshot, xform_node);

    gsk_render_node_unref(xform_node);
    gsk_render_node_unref(color_node);
    gsk_transform_unref(xform);
}

void rect(struct GraphicsContext gc, double x, double y, double w, double h) {
    GtkSnapshot *snapshot = (GtkSnapshot *)gc.native_context;
    if (!snapshot) return;

    graphene_rect_t r = GRAPHENE_RECT_INIT((float)x, (float)y, (float)w, (float)h);
    GdkRGBA color = {1.0f, 0.0f, 0.0f, 1.0f}; /* red */

    gtk_snapshot_append_color(snapshot, &color, &r);
}

/* ── View painting ───────────────────────────────────────────────── */

/*
 * Trampoline for CompositeView_paint: signature must match
 *   void (*)(void *context, struct View *view)
 */
static void paint_view_trampoline(void *context, struct View *view) {
    paint_view((GtkSnapshot *)context, view);
}

static void paint_view(GtkSnapshot *snapshot, struct View *view) {
    if (!view) return;

    switch (view->type) {
    case CustomView: {
        struct CustomView *cv = (struct CustomView *)view;
        if (cv->paint) {
            struct GraphicsContext gc = {
                .width          = (double)view->width,
                .height         = (double)view->height,
                .native_context = snapshot,
            };
            cv->paint(gc);
        }
        break;
    }
    case CompositeView: {
        struct CompositeView *comp = (struct CompositeView *)view;
        CompositeView_paint(comp, snapshot, paint_view_trampoline);
        break;
    }
    default:
        break;
    }
}

/* ── GTK application lifecycle ───────────────────────────────────── */

static void on_activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "cxpui \xe2\x80\x93 GTK4");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    g_canvas = cxpui_canvas_new();

    /* Let the canvas expand to fill the window. */
    gtk_widget_set_hexpand(g_canvas, TRUE);
    gtk_widget_set_vexpand(g_canvas, TRUE);

    gtk_window_set_child(GTK_WINDOW(window), g_canvas);

    if (g_mainWindow) {
        g_mainWindow->native_view = window;
    }

    gtk_window_present(GTK_WINDOW(window));
}

/* ── Public API (cxpui.h) ────────────────────────────────────────── */

struct Window *start(void) {
    g_app = gtk_application_new("at.yeoman.cxpui.gtk4",
                                G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(g_app, "activate", G_CALLBACK(on_activate), NULL);

    struct Window *w = allocate(sizeof(struct Window));
    *w = (struct Window){0};
    w->base.type   = Window;
    w->base.width  = 800;
    w->base.height = 600;
    w->mainView    = NULL;

    g_mainWindow = w;
    return w;
}

void Application_run(void) {
    int status = g_application_run(G_APPLICATION(g_app), 0, NULL);
    g_object_unref(g_app);
    exit(status);
}

struct Window *Window_create(void *native_view) {
    struct Window *w = allocate(sizeof(struct Window));
    *w = (struct Window){0};
    w->base.type   = Window;
    w->native_view = native_view;
    return w;
}

void Window_set_main_view(struct Window *window, struct View *view) {
    window->mainView = view;
    /* g_canvas may be NULL here (called before Application_run).
     * That's fine — on_activate will pick up mainView when it paints. */
    if (g_canvas) {
        gtk_widget_queue_draw(g_canvas);
    }
}

struct NativeView *NativeView_create(void *native_view) {
    struct NativeView *nv = allocate(sizeof(struct NativeView));
    *nv = (struct NativeView){0};
    nv->base.type   = NativeView;
    nv->native_view = native_view;
    return nv;
}
