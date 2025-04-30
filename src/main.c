#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "graphical_interface.h"


int game_in_terminal() {
    
    char **board = gameInitialization();
    if(board == NULL) {
        fprintf(stderr, "Erreur d'initialisation du plateau de jeu.\n");
        return 1;
    }
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
        }

        // Changer de joueur
        current=changePlayer(current);
    }

    // Si match nul
    if (running==1) {
        printBoard(board);
        printf("Match nul !\n");
    }

    // Libération de la mémoire
    freeBoard(board);

    return 0;
}



int main() {
    int choice;
    printf("Choisissez le mode de jeu :\n");
    printf("1. Terminal\n");
    printf("2. Graphique\n");
    printf("Votre choix : ");
    scanf("%d", &choice);


    // Si le choix est 1, on appelle le mode terminal
    if (choice == 1) {
        game_in_terminal();
    }

    // Si le choix est 2, on appelle le mode graphique
    else if (choice == 2) {
        game_in_gui();
        
    }

    else {
        printf("Choix invalide. Veuillez choisir 1 ou 2.\n");
        return 1;
    }

    return 0;
}
