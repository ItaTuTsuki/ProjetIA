#include <stdio.h>
#include <stdlib.h>
#include "game.h"

int main() {
    printf("Bienvenue dans Puissance N !\n");

    // Demander les paramètres de la grille
    printf("Entrez le nombre de lignes (minimum 4) : ");
    scanf("%d", &ROWS);

    printf("Entrez le nombre de colonnes (minimum 4) : ");
    scanf("%d", &COLS);

    printf("Entrez le nombre de jetons à aligner pour gagner (N >= 4) : ");
    scanf("%d", &N);

    // Vérifications de base
    if (ROWS < 4  || COLS < 4 || N < 4) {
        printf("Paramètres invalides. Minimum 4 pour lignes, colonnes et alignement.\n");
        return 1;
    }
    if (N > ROWS && N > COLS) {
        printf("Impossible d'aligner %d jetons sur cette grille. (Grille trop petite)\n", N);
        return 1;
    }

    // Création du plateau
    char **board = createBoard();
    initBoard(board);

    char current = PLAYER; // Joueur X commence
    int col;
    int running = 1;

    // Boucle principale de jeu
    while (running && !boardFull(board)) {
        printBoard(board);
        printf("Joueur %c - Choisissez une colonne (0-%d) : ", current, COLS - 1);
        scanf("%d", &col);

        // Vérification du coup
        while (!isValidMove(board, col)) {
            printf("Coup invalide. Rejouez : ");
            scanf("%d", &col);
        }

        makeMove(board, col, current);

        // Vérifier la victoire
        if (checkWin(board, current)) {
            printBoard(board);
            printf("Le joueur %c a gagné !\n", current);
            running = 0;
            break;
        }

        // Changer de joueur
        current = (current == PLAYER) ? AI : PLAYER;
    }

    // Si match nul
    if (running) {
        printBoard(board);
        printf("Match nul !\n");
    }

    // Libération de la mémoire
    freeBoard(board);

    return 0;
}