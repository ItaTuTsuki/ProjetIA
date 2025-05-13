#include <gtk/gtk.h>
#include <locale.h>
#include "game.h"
#include "graphical_interface.h"

static char **board;
static char current_player = PLAYER;
static GtkWidget *area;

// Animation
static int anim_col = -1;
static int anim_row = -1;
static int anim_current_row = -1;
static gboolean animating = FALSE;
static GtkWidget *anim_label = NULL;

// Scores et tour
static int score_player = 0;
static int score_ai = 0;
static GtkWidget *score_label = NULL;
static GtkWidget *params_label = NULL;
static gboolean player_starts = TRUE;

int getAvailableRow(char **board, int col) {
    for (int i = ROWS - 1; i >= 0; --i) {
        if (board[i][col] == EMPTY) {
            return i;
        }
    }
    return -1;
}

static void update_info_labels() {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Score - <span foreground='red'><b>Rouge: %d</b></span>, <span foreground='gold'><b>Jaune: %d</b></span>",score_player, score_ai);
    gtk_label_set_markup(GTK_LABEL(score_label), buffer);
}

static void start_new_game() {
    initBoard(board);
    player_starts = !player_starts;
    current_player = player_starts ? PLAYER : AI;
    const char *text = (current_player == PLAYER)
        ? "Au tour du joueur <span foreground='red'><b>Rouge</b></span>"
        : "Au tour du joueur <span foreground='gold'><b>Jaune</b></span>";
    gtk_label_set_markup(GTK_LABEL(anim_label), text);
    update_info_labels();
    gtk_widget_queue_draw(area);
}

static gboolean animate_drop(gpointer data) {
    if (anim_current_row > 0)
        board[anim_current_row - 1][anim_col] = EMPTY;

    board[anim_current_row][anim_col] = current_player;
    gtk_widget_queue_draw(area);

    if (anim_current_row < anim_row) {
        anim_current_row++;
        return G_SOURCE_CONTINUE;
    } else {
        animating = FALSE;

        if (checkWin(board, current_player)) {
            if (current_player == PLAYER) score_player++;
            else score_ai++;

            char message[256];
            const char *couleur_current_player = (current_player == PLAYER)
                ? "<span foreground='red'><b>Rouge</b></span>"
                : "<span foreground='gold'><b>Jaune</b></span>";
            snprintf(message, sizeof(message),
                "Le joueur %s a gagn\u00e9 !\nVoulez-vous rejouer ?", couleur_current_player);

            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, NULL);
            gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog), message);

            int response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if (response == GTK_RESPONSE_YES) {
                start_new_game();
            } else {
                gtk_main_quit();
            }
        } else if (boardFull(board)) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO,
                "Match nul !\nVoulez-vous rejouer ?");

            int response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if (response == GTK_RESPONSE_YES) {
                start_new_game();
            } else {
                gtk_main_quit();
            }
        } else {
            current_player = changePlayer(current_player);
            const char *text = (current_player == PLAYER)
                ? "Au tour du joueur <span foreground='red'><b>Rouge</b></span>"
                : "Au tour du joueur <span foreground='gold'><b>Jaune</b></span>";
            gtk_label_set_markup(GTK_LABEL(anim_label), text);
        }

        update_info_labels();
        return G_SOURCE_REMOVE;
    }
}

static void draw_board(GtkWidget *widget, cairo_t *cr, gpointer data) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    cairo_set_source_rgb(cr, 0.2, 0.4, 0.9);
    cairo_paint(cr);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            double x = j * CELL_SIZE + CELL_SIZE / 2.0;
            double y = i * CELL_SIZE + CELL_SIZE / 2.0;

            if (board[i][j] == PLAYER) {
                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
            } else if (board[i][j] == AI) {
                cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
            } else {
                cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            }

            cairo_arc(cr, x, y, CELL_SIZE / 2.5, 0, 2 * G_PI);
            cairo_fill(cr);
        }
    }
}

static gboolean on_click(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_BUTTON_PRESS && !animating) {
        int col = event->x / CELL_SIZE;
        if (isValidMove(board, col)) {
            anim_col = col;
            anim_row = getAvailableRow(board, col);
            anim_current_row = 0;
            animating = TRUE;
            anim_label = GTK_WIDGET(data);
            g_timeout_add(50, animate_drop, NULL);
        }
    }
    return TRUE;
}

int game_in_gui() {
    setlocale(LC_ALL, "fr_FR.UTF-8");
    gtk_init(NULL, NULL);

    board = gameInitialization();
    if (board == NULL) {
        fprintf(stderr, "Erreur d'initialisation du plateau de jeu.\n");
        return 1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Puissance N");

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // ----- Zone info haut de l'interface -----
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), info_box, FALSE, FALSE, 0);

    // Taille de la grille et alignement requis
    params_label = gtk_label_new(NULL);  // Initialisation correcte ici
    char params_buffer[128];
    snprintf(params_buffer, sizeof(params_buffer), "Grille : %d x %d | Jetons pour gagner : %d", ROWS, COLS, ALIGN_TO_WIN);
    gtk_label_set_text(GTK_LABEL(params_label), params_buffer);
    gtk_box_pack_start(GTK_BOX(info_box), params_label, FALSE, FALSE, 0);

    // Score (à mettre à jour ailleurs dans le code après chaque partie)
    score_label = gtk_label_new(NULL);  // Initialisation correcte ici
    char score_buffer[128];
    snprintf(score_buffer, sizeof(score_buffer), "Score - <span foreground='red'><b>Rouge: %d</b></span>, <span foreground='gold'><b>Jaune: %d</b></span>", score_player, score_ai);
    gtk_label_set_markup(GTK_LABEL(score_label), score_buffer);
    gtk_box_pack_start(GTK_BOX(info_box), score_label, FALSE, FALSE, 0);

    // Label pour le tour du joueur
    anim_label = gtk_label_new(NULL);  // Initialisation correcte ici
    gtk_label_set_markup(GTK_LABEL(anim_label), "Au tour du joueur <span foreground='red'><b>Rouge</b></span>");
    gtk_box_pack_start(GTK_BOX(main_vbox), anim_label, FALSE, FALSE, 5);

    // ----- Zone du plateau -----
    area = gtk_drawing_area_new();
    gtk_widget_set_size_request(area, COLS * CELL_SIZE, ROWS * CELL_SIZE);
    gtk_box_pack_start(GTK_BOX(main_vbox), area, TRUE, TRUE, 0);

    // Connexions des signaux
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(area, "draw", G_CALLBACK(draw_board), NULL);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_click), anim_label);
    gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK);

    gtk_widget_show_all(window);
    gtk_main();

    freeBoard(board);
    return 0;
}
