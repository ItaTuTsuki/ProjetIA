#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "graphical_interface.h"
#include "ai.h"

// Fonction principale pour jouer dans le terminal
int game_in_terminal() {
    char **board = gameInitialization(); // Création du plateau de jeu

    if(board == NULL) {
        fprintf(stderr, "Erreur d'initialisation du plateau de jeu.\n");
        return 1;
    }

    int mode;
    // Choix entre deux modes : joueur contre joueur ou contre IA
    printf("Choisissez le mode :\n");
    printf("1. Humain vs Humain\n");
    printf("2. Humain vs IA\n");
    printf("Votre choix : ");
    scanf("%d", &mode);

    char current = PLAYER; // Le joueur 'X' commence
    int col;
    int running = 1;

    // Boucle principale du jeu
    while (running && !boardFull(board)) {
        printBoard(board); // Affiche le plateau

        if (mode == 2 && current == AI) {
            // Tour de l'IA
            printf("Tour de l'IA...\n");
            col = getBestMove(board, 4); // Profondeur de recherche = 4
            printf("L'IA joue en colonne %d\n", col);
        } else {
            // Tour d’un joueur humain
            printf("Joueur %c - Choisissez une colonne (0-%d) : ", current, COLS - 1);
            scanf("%d", &col);

            // Vérifie que le coup est autorisé
            while (!isValidMove(board, col)) {
                printf("Coup invalide. Rejouez : ");
                scanf("%d", &col);
            }
        }

        makeMove(board, col, current); // Joue le coup

        // Vérifie si le joueur courant a gagné
        if (checkWin(board, current)) {
            printBoard(board);
            printf("Le joueur %c a gagné !\n", current);
            running = 0;
        }

        // Passe au joueur suivant
        current = changePlayer(current);
    }

    // Si la boucle s'arrête sans gagnant, c’est un match nul
    if (running) {
        printBoard(board);
        printf("Match nul !\n");
    }

    freeBoard(board); // Libération de la mémoire
    return 0;
}

// Point d'entrée du programme
int main() {
    int choice;

    // Choix du mode d'affichage : terminal ou graphique
    printf("Choisissez le mode de jeu :\n");
    printf("1. Terminal\n");
    printf("2. Graphique\n");
    printf("Votre choix : ");
    scanf("%d", &choice);

    if (choice == 1) {
        game_in_terminal(); // Lance la version console
    } else if (choice == 2) {
        game_in_gui(); // Lance la version graphique
    } else {
        printf("Choix invalide. Veuillez choisir 1 ou 2.\n");
        return 1;
    }

    return 0;
}