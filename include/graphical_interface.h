#ifndef GRAPHICAL_INTERFACE_H
#define GRAPHICAL_INTERFACE_H
#include <gtk/gtk.h>

#define CELL_SIZE 80

int game_in_gui();
gboolean animate_drop();
void draw_board(GtkWidget *widget, cairo_t *cr);
gboolean on_click(__attribute__((unused))GtkWidget *widget ,  GdkEventButton *event);


#endif
