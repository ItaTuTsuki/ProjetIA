// ai.c
#include <limits.h>
#include "game.h"
#include "ai.h"
#include <math.h>

int profondeur = 4; // profondeur de recherche

char getAIPlayer() {
    return AI;
}

int eval_segment(int ai_count, int human_count) {
    if (ai_count > 0 && human_count > 0) return 0; // bloqué
    if (ai_count > 0) return pow(10,ai_count-1);
    if(human_count > 0) return -pow(10,human_count-1);
    return 0;
}

static int evaluate_board(char **board, char ai_player, char human_player) {
    int score = 0;

    // Centre
    for (int row = 0; row < ROWS; row++) {
        if (board[row][COLS / 2] == ai_player){
            score += 3;
        }
    }

    // Alignements horizontaux
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS - ALIGN_TO_WIN -1; col++) {
            int ai_count = 0, human_count = 0;
            for (int i = 0; i < ALIGN_TO_WIN; i++) {
                if (board[row][col + i] == ai_player) ai_count++;
                if (board[row][col + i] == human_player) human_count++;
            }
            score += eval_segment(ai_count, human_count);
        }
    }

    // Alignements verticaux
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS - ALIGN_TO_WIN -1; row++) {
            int ai_count = 0, human_count = 0;
            for (int i = 0; i < ALIGN_TO_WIN; i++) {
                if (board[row + i][col] == ai_player) ai_count++;
                if (board[row + i][col] == human_player) human_count++;
            }
            score += eval_segment(ai_count, human_count);
        }
    }

    // Diagonales descendantes
    for (int row = 0; row < ROWS - ALIGN_TO_WIN -1; row++) {
        for (int col = 0; col < COLS - ALIGN_TO_WIN -1; col++) {
            int ai_count = 0, human_count = 0;
            for (int i = 0; i < ALIGN_TO_WIN; i++) {
                if (board[row + i][col + i] == ai_player) ai_count++;
                if (board[row + i][col + i] == human_player) human_count++;
            }
            score += eval_segment(ai_count, human_count);
        }
    }

    // Diagonales montantes
    for (int row = ALIGN_TO_WIN -1; row < ROWS; row++) {
        for (int col = 0; col < COLS - ALIGN_TO_WIN -1; col++) {
            int ai_count = 0, human_count = 0;
            for (int i = 0; i < ALIGN_TO_WIN; i++) {
                if (board[row - i][col + i] == ai_player) ai_count++;
                if (board[row - i][col + i] == human_player) human_count++;
            }
            score += eval_segment(ai_count, human_count);
        }
    }

    return score;
}

void undoMove(char **board, int col) {
    for (int i = 0; i < ROWS; i++) {
        if (board[i][col] != EMPTY) {
            board[i][col] = EMPTY;
            break;
        }
    }
}

int minimax(char **board, int depth, int maximizingPlayer, char ai_player, char human_player) {
    if (checkWin(board, ai_player)) return 100000;
    if (checkWin(board, human_player)) return -100000;
    if (depth == 0 || boardFull(board)) {
        return evaluate_board(board, ai_player, human_player);
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int col = 0; col < COLS; col++) {
            if (isValidMove(board, col)) {
                makeMove(board, col, ai_player);
                int eval = minimax(board, depth - 1, 0, ai_player, human_player);
                undoMove(board, col);
                if (eval > maxEval) maxEval = eval;
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int col = 0; col < COLS; col++) {
            if (isValidMove(board, col)) {
                makeMove(board, col, human_player);
                int eval = minimax(board, depth - 1, 1, ai_player, human_player);
                undoMove(board, col);
                if (eval < minEval) minEval = eval;
            }
        }
        return minEval;
    }
}

int getBestMove(char **board, int depth) {
    char ai_player = AI;
    char human_player = PLAYER;

    // Coup gagnant immédiat
    for (int col = 0; col < COLS; col++) {
        if (isValidMove(board, col)) {
            makeMove(board, col, ai_player);
            if (checkWin(board, ai_player)) {
                undoMove(board, col);
                return col;
            }
            undoMove(board, col);
        }
    }

    // Bloquer joueur s'il peut gagner au prochain coup
    for (int col = 0; col < COLS; col++) {
        if (isValidMove(board, col)) {
            makeMove(board, col, human_player);
            if (checkWin(board, human_player)) {
                undoMove(board, col);
                return col;
            }
            undoMove(board, col);
        }
    }

    // Minimax
    int bestScore = INT_MIN;
    int bestCol = -1;

    for (int col = 0; col < COLS; col++) {
        if (isValidMove(board, col)) {
            makeMove(board, col, ai_player);
            int score = minimax(board, depth - 1, 0, ai_player, human_player);
            undoMove(board, col);
            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;
}
