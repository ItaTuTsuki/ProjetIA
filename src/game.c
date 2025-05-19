#include <stdio.h>   
#include <stdlib.h> 
#include "game.h"      

// Déclaration de variables globales pour la configuration du jeu
int ROWS = 6;              // Nombre de lignes de la grille (modifiable par l'utilisateur)
int COLS = 7;              // Nombre de colonnes de la grille
int ALIGN_TO_WIN = 4;      // Nombre de jetons à aligner pour gagner

// Fonction qui crée dynamiquement une grille de jeu vide
char **createBoard() {
    // Allocation d'un tableau de pointeurs (une ligne par pointeur)
    char **board = malloc(ROWS * sizeof(char *));
    if (!board) {
        perror("Erreur allocation board"); // Affiche une erreur système si malloc échoue
        exit(EXIT_FAILURE);                // Arrête le programme
    }

    // Allocation de chaque ligne du tableau (chaque ligne a COLS colonnes)
    for (int i = 0; i < ROWS; i++) {
        board[i] = malloc(COLS * sizeof(char));
        if (!board[i]) {
            perror("Erreur allocation ligne board");
            exit(EXIT_FAILURE);
        }
    }

    return board; // Retourne le pointeur vers la grille
}

// Libère la mémoire allouée dynamiquement pour la grille
void freeBoard(char **board) {
    for (int i = 0; i < ROWS; i++) {
        free(board[i]); // Libère chaque ligne
    }
    free(board);        // Libère le tableau de pointeurs
}

// Initialise chaque case de la grille avec le caractère vide
void initBoard(char **board) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            board[i][j] = EMPTY; 
}

// Affiche la grille dans le terminal
void printBoard(char **board) {
    printf("\n");
    for (int i = 0; i < ROWS; i++) {
        printf("|");
        for (int j = 0; j < COLS; j++) {
            printf(" %c |", board[i][j]); // Affiche chaque case avec des séparateurs
        }
        printf("\n");
    }

    // Affiche les indices des colonnes pour aider le joueur
    printf(" ");
    for (int j = 0; j < COLS; j++) {
        printf(" %d  ", j);
    }
    printf("\n\n");
}

// Vérifie si un coup est valide
int isValidMove(char **board, int col) {
    return (col >= 0 && col < COLS && board[0][col] == EMPTY); // On regarde si la case du haut est vide
}

// Insère un pion dans la colonne spécifiée, le plus bas possible
int makeMove(char **board, int col, char token) {
    for (int i = ROWS - 1; i >= 0; i--) { // On part du bas de la grille
        if (board[i][col] == EMPTY) {     // Si la case est vide
            board[i][col] = token;        // On place le pion du joueur
            return 1;                     // Coup réussi
        }
    }
    return 0; // Colonne pleine
}

// Vérifie si la grille est pleine
int boardFull(char **board) {
    for (int j = 0; j < COLS; j++) {
        if (board[0][j] == EMPTY) // Si une case du haut est vide, la grille n'est pas pleine
            return 0;
    }
    return 1; // Aucune case vide en haut : la grille est pleine
}

// Vérifie si un joueur a gagné (alignement de jetons)
int checkWin(char **board, char token) {
    // Vérifie les alignements horizontaux
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j <= COLS - ALIGN_TO_WIN; j++) {
            int count = 0;
            for (int k = 0; k < ALIGN_TO_WIN; k++) {
                if (board[i][j + k] == token)
                    count++;
            }
            if (count == ALIGN_TO_WIN) return 1; // Alignement trouvé
        }
    }

    // Vérifie les alignements verticaux
    for (int j = 0; j < COLS; j++) {
        for (int i = 0; i <= ROWS - ALIGN_TO_WIN; i++) {
            int count = 0;
            for (int k = 0; k < ALIGN_TO_WIN; k++) {
                if (board[i + k][j] == token)
                    count++;
            }
            if (count == ALIGN_TO_WIN) return 1;
        }
    }

    // Vérifie les diagonales ↘ (haut-gauche vers bas-droit)
    for (int i = 0; i <= ROWS - ALIGN_TO_WIN; i++) {
        for (int j = 0; j <= COLS - ALIGN_TO_WIN; j++) {
            int count = 0;
            for (int k = 0; k < ALIGN_TO_WIN; k++) {
                if (board[i + k][j + k] == token)
                    count++;
            }
            if (count == ALIGN_TO_WIN) return 1;
        }
    }

    // Vérifie les diagonales ↙ (haut-droit vers bas-gauche)
    for (int i = 0; i <= ROWS - ALIGN_TO_WIN; i++) {
        for (int j = ALIGN_TO_WIN - 1; j < COLS; j++) {
            int count = 0;
            for (int k = 0; k < ALIGN_TO_WIN; k++) {
                if (board[i + k][j - k] == token)
                    count++;
            }
            if (count == ALIGN_TO_WIN) return 1;
        }
    }

    return 0; // Aucun alignement trouvé
}

// Change le joueur courant
char changePlayer(char current) {
    return (current == PLAYER) ? AI : PLAYER;
}

// Fonction utilitaire qui retourne le maximum entre deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Initialise la partie : demande les paramètres de la grille à l'utilisateur
char** gameInitialization() {
    printf("Bienvenue dans Puissance N !\n");

    // L'utilisateur choisit les dimensions de la grille et l'alignement
    printf("Entrez le nombre de lignes (minimum 4) : ");
    scanf("%d", &ROWS);

    printf("Entrez le nombre de colonnes (minimum 4) : ");
    scanf("%d", &COLS);

    // Demande le nombre de jetons à aligner pour gagner, en respectant une borne maximale
    printf("Entrez le nombre de jetons à aligner pour gagner (3 ≤ ALIGN_TO_WIN ≤ %d) : ", max(ROWS, COLS));
    scanf("%d", &ALIGN_TO_WIN);

    // Vérifie la validité des valeurs
    if (ROWS < 4  || COLS < 4 || ALIGN_TO_WIN < 3) {
        printf("Paramètres invalides. Minimum 4 pour lignes, colonnes et alignement.\n");
        return NULL;
    }

    if (ALIGN_TO_WIN > ROWS && ALIGN_TO_WIN > COLS) {
        printf("Impossible d'aligner %d jetons sur cette grille. (Grille trop petite)\n", ALIGN_TO_WIN);
        return NULL;
    }

    // Crée et initialise la grille de jeu
    char **board = createBoard();
    initBoard(board);

    return board; // Retourne la grille prête à être utilisée
}
