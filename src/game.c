#include <stdio.h>
#include <stdlib.h>
#include "game.h"

// Définition des variables globales
int ROWS = 6;
int COLS = 7;
int N = 4;

// Crée dynamiquement la grille de jeu
char **createBoard() {
    char **board = malloc(ROWS * sizeof(char *));
    if (!board) {
        perror("Erreur allocation board");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ROWS; i++) {
        board[i] = malloc(COLS * sizeof(char));
        if (!board[i]) {
            perror("Erreur allocation ligne board");
            exit(EXIT_FAILURE);
        }
    }
    return board;
}

// Libère dynamiquement la grille
void freeBoard(char **board) {
    for (int i = 0; i < ROWS; i++) {
        free(board[i]);
    }
    free(board);
}

// Initialise la grille vide
void initBoard(char **board) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            board[i][j] = EMPTY;
}

// Affiche la grille sur le terminal
void printBoard(char **board) {
    printf("\n");
    for (int i = 0; i < ROWS; i++) {
        printf("|");
        for (int j = 0; j < COLS; j++) {
            printf(" %c |", board[i][j]);
        }
        printf("\n");
    }
    printf(" ");
    for (int j = 0; j < COLS; j++) {
        printf(" %d  ", j);
    }
    printf("\n\n");
}

// Vérifie si un coup est valide
int isValidMove(char **board, int col) {
    return (col >= 0 && col < COLS && board[0][col] == EMPTY);
}

// Joue un coup
int makeMove(char **board, int col, char token) {
    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i][col] == EMPTY) {
            board[i][col] = token;
            return 1;
        }
    }
    return 0; // Colonne pleine
}

// Vérifie si la grille est pleine
int boardFull(char **board) {
    for (int j = 0; j < COLS; j++) {
        if (board[0][j] == EMPTY)
            return 0;
    }
    return 1;
}

// Vérifie s'il y a une victoire
int checkWin(char **board, char token) {
    // Horizontal
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j <= COLS - N; j++) {
            int count = 0;
            for (int k = 0; k < N; k++) {
                if (board[i][j + k] == token)
                    count++;
            }
            if (count == N) return 1;
        }
    }

    // Vertical
    for (int j = 0; j < COLS; j++) {
        for (int i = 0; i <= ROWS - N; i++) {
            int count = 0;
            for (int k = 0; k < N; k++) {
                if (board[i + k][j] == token)
                    count++;
            }
            if (count == N) return 1;
        }
    }

    // Diagonale ↘
    for (int i = 0; i <= ROWS - N; i++) {
        for (int j = 0; j <= COLS - N; j++) {
            int count = 0;
            for (int k = 0; k < N; k++) {
                if (board[i + k][j + k] == token)
                    count++;
            }
            if (count == N) return 1;
        }
    }

    // Diagonale ↙
    for (int i = 0; i <= ROWS - N; i++) {
        for (int j = N - 1; j < COLS; j++) {
            int count = 0;
            for (int k = 0; k < N; k++) {
                if (board[i + k][j - k] == token)
                    count++;
            }
            if (count == N) return 1;
        }
    }

    return 0;
}

// Change le joueur courant
char changePlayer(char current) {
    if (current == PLAYER) {
        return AI;
    } else {
        return PLAYER;
    }
}



char** gameInitialization() {
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
        return NULL;
    }
    if (N > ROWS && N > COLS) {
        printf("Impossible d'aligner %d jetons sur cette grille. (Grille trop petite)\n", N);
        return NULL;
    }

    // Création du plateau
    char **board = createBoard();
    initBoard(board);

    return board;
}
