#pragma once

#include <gtk/gtk.h>

#define GRAPH_TYPE_WIDGET (basic_example_get_type ())
G_DECLARE_FINAL_TYPE (BasicExample, basic_example, GRAPH, WIDGET, GtkWidget)

GtkWidget * basic_example_new       (void);
