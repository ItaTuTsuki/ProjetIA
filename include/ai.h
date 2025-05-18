#ifndef AI_H
#define AI_H


extern int profondeur; // profondeur de recherche
char getAIPlayer(); // retourne 'O' (IA)
int getBestMove(char **board, int depth); // retourne la meilleure colonne pour jouer
int evaluateBoard(char **board); // évalue le plateau pour l’IA

#endif