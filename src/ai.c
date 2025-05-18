// ai.c
#include <limits.h>
#include "game.h"
#include "ai.h"

char getAIPlayer() {
    return AI;
}

// Fonction d'évaluation de base
int evaluateBoard(char **board) {
    if (checkWin(board, AI)) return 10;
    if (checkWin(board, PLAYER)) return -10;
    return 0;
}

// Fonction pour annuler un coup (dernier jeton dans une colonne)
void undoMove(char **board, int col) {
    for (int i = 0; i < ROWS; i++) {
        if (board[i][col] != EMPTY) {
            board[i][col] = EMPTY;
            break;
        }
    }
}

// Minimax sans élagage alpha-bêta
int minimax(char **board, int depth, int maximizingPlayer) {
    int score = evaluateBoard(board);
    if (score != 0 || depth == 0 || boardFull(board)) {
        return score;
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int col = 0; col < COLS; col++) {
            if (isValidMove(board, col)) {
                makeMove(board, col, AI);
                int eval = minimax(board, depth - 1, 0);
                undoMove(board, col);
                if (eval > maxEval) maxEval = eval;
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int col = 0; col < COLS; col++) {
            if (isValidMove(board, col)) {
                makeMove(board, col, PLAYER);
                int eval = minimax(board, depth - 1, 1);
                undoMove(board, col);
                if (eval < minEval) minEval = eval;
            }
        }
        return minEval;
    }
}

// Trouve le meilleur coup pour l'IA
int getBestMove(char **board, int depth) {
    int bestScore = INT_MIN;
    int bestCol = -1;

    for (int col = 0; col < COLS; col++) {
        if (isValidMove(board, col)) {
            makeMove(board, col, AI);
            int score = minimax(board, depth - 1, 0);
            undoMove(board, col);
            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;
}
