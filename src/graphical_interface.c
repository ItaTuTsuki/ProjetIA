#include <gtk/gtk.h>    //Interfaces graphiques
#include <string.h>
#include "game.h"       //Logique du jeu
#include "graphical_interface.h"
#include "ai.h"         //IA

//Variables globales du jeu
char **board;
char current_player = PLAYER;
GtkWidget *area;
int hover_col = -1;


//Variables globnales pour gérer l'animation de chute des jetons
int anim_row = -1;
int anim_col = -1;
int anim_current_row = -1;
gboolean animating = FALSE;
GtkWidget *anim_label = NULL;

//Variables globales pour le score et les paramètres
int score_player = 0;
int score_ai = 0;
int mode_vs_ai = 1;
GtkWidget *score_label = NULL;
GtkWidget *params_label = NULL;
gboolean player_starts = TRUE;


//Fonction pour dessiner le plateau et gère le survolement de la colonne
//Est appelée à chaque fois que le plateau doit être redessiné
void draw_board(GtkWidget *widget, cairo_t *cr) {
    //Récupère la taille et la position du widget pour connaître la surface à dessiner.
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    //Définit la couleur de fond en bleu et dessine le rectangle
    cairo_set_source_rgb(cr, 0.2, 0.4, 0.9);
    cairo_paint(cr);

    //Dessine la grille avec les jetons de couleur rouge et jaune et les cases vides en blanc
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            //Calcul la position du centre du jeton à placer
            double x = (j * CELL_SIZE) + (CELL_SIZE / 2.0);
            double y = (i * CELL_SIZE) + (CELL_SIZE / 2.0);

            //Définit la couleur du jeton selon le joueur
            if (board[i][j] == PLAYER) {
                // Rouge pour le joueur humain (ou joueur 1)
                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
            } else if (board[i][j] == AI) {
                // Jaune pour l'IA (ou joueur 2)
                cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
            } else {
                // Blanc pour les cases vides
                cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            }

            //Dessine le jeton dans la case avec la couleur appropriée
            cairo_arc(cr, x, y, CELL_SIZE / 2.5, 0, 2 * G_PI);
            cairo_fill(cr);
        }
    }
    //Dessine le jeton en survol (s'il y a une colonne survolée et pas en animation)
    if (hover_col >= 0 /*regarde si une colone est survolée de base hover_col=-1*/ && !animating) {
        //Calcul la position du centre du jeton à placer en haut de la colonne survolée
        double x = (hover_col * CELL_SIZE) + (CELL_SIZE / 2.0);
        double y = CELL_SIZE / 2.0;

        //Définit la couleur du jeton selon le joueur
        if (current_player == PLAYER)
            cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);  // Rouge
        else
            cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);  // Jaune

        //Dessine le jeton en haut de la colonne survolée
        cairo_arc(cr, x, y, CELL_SIZE / 2.5, 0, 2 * G_PI);
        cairo_fill(cr);
    }
}

//Récupère la ligne disponible pour placer un jeton dans la colonne donnée
int getAvailableRow(char **board, int col) {
    //Regarde la colonne de bas en haut
    for (int i = ROWS - 1; i >= 0; --i) {
        if (board[i][col] == EMPTY) {
            // Si la case est vide, retourne la ligne
            return i;
        }
    }
    //Si la colonne est pleine, retourne -1
    return -1;
}

//Fonction appelée lors du clic sur le plateau de la souris
gboolean on_click(__attribute__((unused))GtkWidget *widget , GdkEventButton *event /*obtenir la colonne cliquée*/) {
    //verifie si c'est bien un clic et qu'il n'y a pas d'animation en cours
    if (event->type == GDK_BUTTON_PRESS && !animating) {
        //Récupère la colonne cliquée
        int col = event->x / CELL_SIZE;
        
        //Vérifie que le coup est valide
        if (isValidMove(board, col)) {
            //Stocke la colonne et la ligne de l'animation
            anim_col = col;
            anim_row = getAvailableRow(board, col); // Récupère la ligne disponible

            anim_current_row = 0; //Remet à zéro la ligne courante de l'animation
            animating = TRUE; //Active le mode animation
            //Lance l'animation de chute toutes les 50ms
            g_timeout_add(50, animate_drop, NULL);
        }
    }
    //Indique que l'évenement a été traité
    return TRUE;
}

//Fonction appelée lors du mouvement de la souris sur le plateau
//Met à jour la colonne survolée et redessine le plateau si la souris a changé de colonne
gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event) {
    //Calcul la nouvelle colonne survolée
    int new_col = event->x / CELL_SIZE;

    //Si la colonne survolée est différente de l'ancienne et qu'elle est valide
    if (new_col != hover_col && new_col >= 0 && new_col < COLS) {
        //Met à jour la colonne survolée
        hover_col = new_col;
        //Demande un redessin du plateau
        gtk_widget_queue_draw(widget);
    }
    return TRUE;
}

//Fonctions pour colorer le texte en rouge dans la console
char* rouge(const char* texte) {
    const char* prefix = "\033[1;31m";
    const char* suffix = "\033[0m";
    size_t taille = strlen(prefix) + strlen(texte) + strlen(suffix) + 1;
    char* resultat = malloc(taille);
    if (resultat == NULL) return NULL;
    snprintf(resultat, taille, "%s%s%s", prefix, texte, suffix);
    return resultat;
}

//Fonctions pour colorer le texte en jaune dans la console
char* jaune(const char* texte) {
    const char* prefix = "\033[1;33m";
    const char* suffix = "\033[0m";
    size_t taille = strlen(prefix) + strlen(texte) + strlen(suffix) + 1;
    char* resultat = malloc(taille);
    if (resultat == NULL) return NULL;
    snprintf(resultat, taille, "%s%s%s", prefix, texte, suffix);
    return resultat;
}

//Fonction pour mettre à jour l'affichage du score
void update_info_labels() {
    char score_string[128];
    //Formate la chaîne de score en y insérant les scores actuels des joueurs
    //La balise <span foreground='red'> permet de colorer le texte en rouge dans l'interface graphique
    //La balise <b> permet de mettre le texte en gras pour faire ressortir le score
    snprintf(score_string, sizeof(score_string), "Score - <span foreground='red'><b>Rouge: %d</b></span>, <span foreground='gold'><b>Jaune: %d</b></span>",score_player, score_ai);
    //applique le formatage de la chaîne de score à l'étiquette de score 
    //utlisation de la fonction markup et non text pour les couleurs et styles 
    gtk_label_set_markup(GTK_LABEL(score_label), score_string);
}


//Fonction pour démarrer une nouvelle partie dans l'interface graphique
void start_new_game() {
    //initialise le plateau de jeu
    initBoard(board);
    //Alterne l'ordre des joueurs, le joueur qui a commencé la partie précédente ne commence pas la nouvelle
    player_starts = !player_starts;
    current_player = player_starts ? PLAYER : AI;

    //Prépare le texte d'inidication du joueur commençant la partie
    const char *text = (current_player == PLAYER)
        ? "Au tour du joueur <span foreground='red'><b>Rouge</b></span>"
        : "Au tour du joueur <span foreground='gold'><b>Jaune</b></span>";

    //Affiche le texte d'indication du joueur commençant la partie
    gtk_label_set_markup(GTK_LABEL(anim_label), text);

    //Met à jour l'affichage du score
    update_info_labels();

    //Dessine ou Redessine le plateau 
    gtk_widget_queue_draw(area);

    //Si on est en mode IA et que c'est le tour de l'IA de commencé, on lance l'IA
    if (mode_vs_ai && current_player == AI) {
        //Prépare le texte d'indication de l'IA réfléchissant
        gtk_label_set_markup(GTK_LABEL(anim_label), "L'IA réfléchit...");
        while (gtk_events_pending()) gtk_main_iteration();

        //Lance l'IA pour choisir le meilleur coup avec la profondeur choisie
        //Peut prendre un certain temps selon la profondeur de recherche et la taille du plateau
        int ai_col = getBestMove(board, profondeur);
        if (isValidMove(board, ai_col)) {
            //Prépare l'animation de chute du jeton comme précédemment
            anim_col = ai_col;
            anim_row = getAvailableRow(board, ai_col);
            anim_current_row = 0;
            animating = TRUE;
            g_timeout_add(50, animate_drop, NULL);
        }
    }
}

//Fonction qui anime la chute du jeton dans le plateau et gère les événements:
// - Vérifie si le joueur a gagné/perdu
// - Vérifie si le plateau est plein (match nul)
// - Change de joueur

gboolean animate_drop() {
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
                "Le joueur %s a gagné !\nVoulez-vous rejouer ?", couleur_current_player);

            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, NULL);
            gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog), message);

            int response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if (response == GTK_RESPONSE_YES) {
                start_new_game();
            } else {
                char red[128];
                char yellow[128];
                snprintf(red, sizeof(red),
                    "Rouge : %d", score_player);
                snprintf(yellow, sizeof(yellow),
                    "Jaune : %d", score_ai);
                printf("\n\nScore final - %s, %s\n", rouge(red), jaune(yellow));
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
                char red[128];
                char yellow[128];
                snprintf(red, sizeof(red),
                    "Rouge : %d", score_player);
                snprintf(yellow, sizeof(yellow),
                    "Jaune : %d", score_ai);
                printf("\n\nScore final - %s, %s\n", rouge(red), jaune(yellow));
                gtk_main_quit();
            }
        } else {
            current_player = changePlayer(current_player);
            const char *text = (current_player == PLAYER)
                ? "Au tour du joueur <span foreground='red'><b>Rouge</b></span>"
                : "Au tour du joueur <span foreground='gold'><b>Jaune</b></span>";
            gtk_label_set_markup(GTK_LABEL(anim_label), text);

            if (mode_vs_ai && current_player == AI) {
                gtk_label_set_markup(GTK_LABEL(anim_label), "L'IA réfléchit...");
                while (gtk_events_pending()) gtk_main_iteration();

                int ai_col = getBestMove(board, profondeur);
                if (isValidMove(board, ai_col)) {
                    anim_col = ai_col;
                    anim_row = getAvailableRow(board, ai_col);
                    anim_current_row = 0;
                    animating = TRUE;
                    g_timeout_add(50, animate_drop, NULL);
                }
            }
        }

        update_info_labels();
        hover_col = -1;

        return G_SOURCE_REMOVE;
    }
}

int game_in_gui() {
    printf("Mode Graphique - Choisissez le mode :\n");
    printf("1. Humain vs Humain\n");
    printf("2. Humain vs IA\n");
    printf("Votre choix : ");
    scanf("%d", &mode_vs_ai);
    if (mode_vs_ai==2){
        printf("A quelle profondeur l'IA va tester les coups ? (2-6) : ");
        scanf("%d", &profondeur);
        if (profondeur < 2 || profondeur > 6) {
            printf("Profondeur invalide. Utilisation de la profondeur par défaut (4).\n");
            profondeur = 4;
        }
    }
    mode_vs_ai = (mode_vs_ai == 2); // corriger la valeur
    

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

    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), info_box, FALSE, FALSE, 0);

    params_label = gtk_label_new(NULL);
    char params_buffer[128];
    snprintf(params_buffer, sizeof(params_buffer), "Grille : %d x %d | Jetons pour gagner : %d", ROWS, COLS, ALIGN_TO_WIN);
    gtk_label_set_text(GTK_LABEL(params_label), params_buffer);
    gtk_box_pack_start(GTK_BOX(info_box), params_label, FALSE, FALSE, 0);

    score_label = gtk_label_new(NULL);
    char score_buffer[128];
    snprintf(score_buffer, sizeof(score_buffer), "Score - <span foreground='red'><b>Rouge: %d</b></span>, <span foreground='gold'><b>Jaune: %d</b></span>", score_player, score_ai);
    gtk_label_set_markup(GTK_LABEL(score_label), score_buffer);
    gtk_box_pack_start(GTK_BOX(info_box), score_label, FALSE, FALSE, 0);

    anim_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(anim_label), "Au tour du joueur <span foreground='red'><b>Rouge</b></span>");
    gtk_box_pack_start(GTK_BOX(main_vbox), anim_label, FALSE, FALSE, 5);

    area = gtk_drawing_area_new();
    gtk_widget_set_size_request(area, COLS * CELL_SIZE, ROWS * CELL_SIZE);
    gtk_box_pack_start(GTK_BOX(main_vbox), area, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(area, "draw", G_CALLBACK(draw_board), NULL);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_click), anim_label);
    gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK);

    gtk_widget_add_events(area, GDK_POINTER_MOTION_MASK);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(on_mouse_move), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    freeBoard(board);
    return 0;
}
