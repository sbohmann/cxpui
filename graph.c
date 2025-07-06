#include "graph.h"

struct _BasicExample
{
  GtkWidget parent_instance;

  GskPath *path;
  GskStroke *stroke;
  GdkRGBA color;

  guint tick_cb;
  guint64 start_time;

  double period;
  double amplitude;
};

struct _BasicExampleClass
{
  GtkWidgetClass parent_class;
};

G_DEFINE_TYPE (BasicExample, basic_example, GTK_TYPE_WIDGET)

static void
update_path (BasicExample *self,
             float        amplitude)
{
  graphene_point_t p[20];
  GskPathBuilder *builder;

  g_clear_pointer (&self->path, gsk_path_unref);

  for (int i = 0; i < 20; i++)
    {
      p[i].x = 10 * i;
      p[i].y = 50;

      if (i % 4 == 1 || i % 4 == 2)
        {
          if (i % 8 < 4)
            p[i].y += amplitude;
          else
            p[i].y -= amplitude;
        }
    }

  builder = gsk_path_builder_new ();
  gsk_path_builder_move_to (builder, p[0].x, p[0].y);

  for (int i = 0; i < 20; i += 4)
    gsk_path_builder_cubic_to (builder,
                               p[i+1].x, p[i+1].y,
                               p[i+2].x, p[i+2].y,
                               p[i+3].x, p[i+3].y);

  self->path = gsk_path_builder_free_to_path (builder);
}

static gboolean
tick_cb (GtkWidget     *widget,
         GdkFrameClock *frame_clock,
         gpointer       user_data)
{
  BasicExample *self = GRAPH_WIDGET (widget);
  guint64 now;
  double angle;

  now = gdk_frame_clock_get_frame_time (frame_clock);

  if (self->start_time == 0)
    self->start_time = now;

  angle = 360 * (now - self->start_time) / (double)(self->period * G_TIME_SPAN_MINUTE);
  update_path (self, sin (angle) * self->amplitude);

  gtk_widget_queue_draw (widget);

  return G_SOURCE_CONTINUE;
}

static void
basic_example_init (BasicExample *self)
{
  self->color.red = g_random_double_range (0, 1);
  self->color.green = g_random_double_range (0, 1);
  self->color.blue = g_random_double_range (0, 1);
  self->color.alpha = 1;

  self->period = g_random_double_range (0.5, 1);
  self->amplitude = g_random_double_range (10, 25);

  self->stroke = gsk_stroke_new (2);

  update_path (self, 0);

  self->start_time = 0;
  self->tick_cb = gtk_widget_add_tick_callback (GTK_WIDGET (self), tick_cb, NULL, NULL);
}

static void
basic_example_dispose (GObject *object)
{
  BasicExample *self = GRAPH_WIDGET (object);

  g_clear_pointer (&self->path, gsk_path_unref);
  gsk_stroke_free (self->stroke);

  G_OBJECT_CLASS (basic_example_parent_class)->dispose (object);
}

static void
basic_example_snapshot (GtkWidget   *widget,
                      GtkSnapshot *snapshot)
{
  BasicExample *self = GRAPH_WIDGET (widget);

  gtk_snapshot_append_stroke (snapshot, self->path, self->stroke, &self->color);
}

static void
basic_example_measure (GtkWidget      *widget,
                      GtkOrientation  orientation,
                      int             for_size,
                      int            *minimum,
                      int            *natural,
                      int            *minimum_baseline,
                      int            *natural_baseline)
{
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    *minimum = *natural = 200;
  else
    *minimum = *natural = 100;
}

static void
basic_example_class_init (BasicExampleClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = basic_example_dispose;

  widget_class->snapshot = basic_example_snapshot;
  widget_class->measure = basic_example_measure;
}

GtkWidget *
basic_example_new (void)
{
  return g_object_new (basic_example_get_type(), NULL);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
  // Create the application window
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "GTK4 Graphics Example");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  // Create a GtkDrawingArea as the main content of the window
  GtkWidget *drawing_widget = basic_example_new();
  // gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_widget), 400);
  // gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_widget), 300);

  gtk_window_set_child(GTK_DRAWING_AREA(window), drawing_widget);

  // Connect the snapshot signal to render the custom graphics
  g_signal_connect(drawing_widget, "snapshot", G_CALLBACK(basic_example_snapshot), NULL);

  // Show the window
  gtk_widget_set_visible(window, true);
}

// Main function to create the custom widget and display it in a window
int main(int argc, char *argv[]) {   // Create a new application
  GtkApplication *app = gtk_application_new("at.yeoman.cxpui.gtk4", G_APPLICATION_DEFAULT_FLAGS);

  // Connect the "activate" event
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  // Run the application
  int status = g_application_run(G_APPLICATION(app), 0, NULL);

  // Clean up and exit
  g_object_unref(app);
  exit(status);
}
