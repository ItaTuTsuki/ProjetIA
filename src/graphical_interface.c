#include <gtk/gtk.h>
#include "game.h"

#define CELL_SIZE 80

static char **board;
static char current_player = PLAYER;
static GtkWidget *area;

static void draw_board(GtkWidget *widget, cairo_t *cr, gpointer data) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    // Fond bleu
    cairo_set_source_rgb(cr, 0.2, 0.4, 0.9);
    cairo_paint(cr);

    // Dessin des cellules
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            double x = j * CELL_SIZE + CELL_SIZE / 2.0;
            double y = i * CELL_SIZE + CELL_SIZE / 2.0;

            // Couleur du jeton
            if (board[i][j] == PLAYER) {
                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // rouge
            } else if (board[i][j] == AI) {
                cairo_set_source_rgb(cr, 1.0, 1.0, 0.0); // jaune
            } else {
                cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // vide (blanc)
            }

            cairo_arc(cr, x, y, CELL_SIZE / 2.5, 0, 2 * G_PI);
            cairo_fill(cr);
        }
    }
}

static gboolean on_click(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_BUTTON_PRESS) {
        int col = event->x / CELL_SIZE;

        if (isValidMove(board, col)) {
            makeMove(board, col, current_player);

            if (checkWin(board, current_player)) {
                GtkWidget *dialog = gtk_message_dialog_new(
                    NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                    GTK_BUTTONS_YES_NO, "Le joueur %c a gagné ! \n Voulez-vous rejouer ?", current_player);
                
                int response = gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);

                if (response == GTK_RESPONSE_YES) {
                    // Réinitialiser le jeu
                    initBoard(board);
                    current_player = PLAYER;
                    // gtk_widget_queue_draw(widget);
                } else {
                    // Quitter le jeu
                    gtk_main_quit();
                }
            }

            // Vérifier si le plateau est plein
            if (boardFull(board)) {
                GtkWidget *dialog = gtk_message_dialog_new(
                    NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                    GTK_BUTTONS_YES_NO, "Match nul ! \n Voulez-vous rejouer ?");
                
                int response = gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);

                if (response == GTK_RESPONSE_YES) {
                    // Réinitialiser le jeu
                    initBoard(board);
                    current_player = PLAYER;
                } else {
                    // Quitter le jeu
                    gtk_main_quit();
                }
            }

            current_player = changePlayer(current_player);
            gtk_widget_queue_draw(widget);
        }
    }
    return TRUE;
}

int game_in_gui() {
    gtk_init(NULL, NULL);

    board = gameInitialization();
    if(board == NULL) {
        fprintf(stderr, "Erreur d'initialisation du plateau de jeu.\n");
        return 1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Puissance N");
    gtk_window_set_default_size(GTK_WINDOW(window), COLS * CELL_SIZE, ROWS * CELL_SIZE);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    area = gtk_drawing_area_new();
    gtk_widget_set_size_request(area, COLS * CELL_SIZE, ROWS * CELL_SIZE);
    gtk_container_add(GTK_CONTAINER(window), area);

    g_signal_connect(area, "draw", G_CALLBACK(draw_board), NULL);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_click), NULL);

    gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK);

    gtk_widget_show_all(window);
    gtk_main();

    freeBoard(board);
    return 0;
}
