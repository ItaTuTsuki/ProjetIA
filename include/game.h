#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

// Variables globales (définies dans game.c)
extern int ROWS;
extern int COLS;
extern int N;

#define EMPTY ' '   // Case vide
#define PLAYER 'X'  // Joueur humain
#define AI 'O'      // IA

// Fonctions de création/suppression du plateau
char createBoard();
void freeBoard(char board);

// Fonctions de gestion du plateau
void initBoard(char board);
void printBoard(char board);
int isValidMove(char board, int col);
int makeMove(char board, int col, char token);
int boardFull(char board);
int checkWin(char board, char token);

#endif // GAME_H