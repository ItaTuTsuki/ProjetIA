// ai.c
#include <limits.h>
#include "game.h"
#include "ai.h"
#include <math.h>

int profondeur = 4; // profondeur de recherche

char getAIPlayer() {
    return AI;

}



//Fonction d'évaluation qui prend un segment de la grille et compte combien de jetons appartiennent à l'IA et au joueur.
int eval_segment(int ai_count, int human_count) {
    if (ai_count > 0 && human_count > 0) return 0;                                  // Si le segment contient des jetons des 2 joueurs, le score vaut 0
    if (ai_count > 0) return pow(ai_count-1,10);                                    // Si seulement l'IA a des jetons, on donne un score positif
    if(human_count > 0) return -pow(human_count-1,10);                              // Si seulement l'humain a des jetons, on donne un score négatif (car dangereux pour l'ia)
    return 0;
}

// Fonction d'évaluation du plateau, appelée à la fin d'un parcours minimax. Cette fonction parcours
// toute la grille pour ajouter un bonus si des jetons IA sont placés au centre (car c'est stratégiquement meilleur),
// analyse tous les alignements possibles (horizontaux, verticaux, diagonales descendantes et montantes) et
// pour chaque alignement, elle compte les jetons de l'IA et les jetons adverses et appelle eval_segment pour ajouter un score global.
// Cette fonction permets de quantifier à quel point une position est bonne pour l'IA

static int evaluate_board(char **board, char ai_player, char human_player) {
    int score = 0;                                                                 // On initialise le score total à 0

    // Centre
    for (int row = 0; row < ROWS; row++) {                                         // On parcours toutes les lignes
        if (board[row][COLS / 2] == ai_player){                                    // Si l'IA a un jeton au centre
            score += 3;                                                            // On lui donne un bonus de 3 points pour chaque jeton dans la colonne centrale
        }                                                                          // parce que statistiquement, le centre est plus stratégique (plus de possibilité d'alignement)
    }


    // Chaque bloc suivant analyse des groupes consécutifs de cases (des segments de longueurs ALIGN_TO_WIN)
    // Pour chaque segment, on compte le nombre de jetons de l'IA et du joueur humain puis on évalue ce segment avec eval_segment().

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

// Cette fonction permets de supprimer le dernier jeton placer dans une colonne. C'est très utile
// pour annuler un coup simulé pendant l'exécution de minimax. Ce qui permets une exploration sans 
// modifier le plateau de jeu réel.
void undoMove(char **board, int col) {
    for (int i = 0; i < ROWS; i++) {
        if (board[i][col] != EMPTY) {
            board[i][col] = EMPTY;
            break;
        }
    }
}


// Fonction récursive minimax qui permets de prendre une décision.
// C'est le coeur de l'IA : il explore tous les coups possibles, en alternant
// joueurs IA/ joueur humain et attributs un score à chaque scénario possible.
int minimax(char **board, int profondeur, int maximizingPlayer, char ai_player, char human_player) {
    if (checkWin(board, ai_player)) return pow(10,ALIGN_TO_WIN);                  // Si l'IA gagne, on retourne un score très élevé
    if (checkWin(board, human_player)) return -pow(10,ALIGN_TO_WIN);              // Si l'humain gagne, on retourne un score très faible
    if (profondeur == 0 || boardFull(board)) {                                    // Si la profondeur est atteinte ou si le plateau est plein, 
        return evaluate_board(board, ai_player, human_player);                    // on évalue le plateau avec evaluate_board
    }

    if (maximizingPlayer) {                                                       // Si c'est le tour de l'IA
        int maxEval = INT_MIN;                                                    // On initialise le score max à une valeur très faible
        for (int col = 0; col < COLS; col++) {                                    // On parcours toutes les colonnes
            if (isValidMove(board, col)) {                                        // Si le coup est valide (c'est à dire que la colonne n'est pas pleine)
                makeMove(board, col, ai_player);                                  // On place le jeton de l'IA
                int eval = minimax(board, profondeur - 1, 0, ai_player, human_player); // On appelle minimax récursivement pour le coup suivant
                undoMove(board, col);                                             // On annule le coup
                if (eval > maxEval) maxEval = eval;                               // On met à jour le score max si le score retourné est supérieur
            }
        }
        return maxEval;                                                           // On retourne le score max
    } else {                                                                      // Si c'est le tour de l'humain
        int minEval = INT_MAX;                                                    // On initialise le score min à une valeur très élevée
        for (int col = 0; col < COLS; col++) {                                    // On parcours toutes les colonnes
            if (isValidMove(board, col)) {                                        // Si le coup est valide
                makeMove(board, col, human_player);                               // On place le jeton de l'humain
                int eval = minimax(board, profondeur - 1, 1, ai_player, human_player); // On appelle minimax récursivement pour le coup suivant
                undoMove(board, col);                                             // On annule le coup
                if (eval < minEval) minEval = eval;                               // On met à jour le score min si le score retourné est inférieur
            }
        }
        return minEval;                                                           // On retourne le score min
    }
}

// C'est la fonction qu'on appeller réellement dans le jeu pour faire jouer l'IA.
// Elle cherche d'abord un coup gagnant immédiat
// puis elle bloque le joueur s'il peut gagner au prochain coup.
// Si aucun des deux n'est possible, elle utilise minimax pour évaluer les coups possibles
int getBestMove(char **board, int profondeur) {
    char ai_player = AI;
    char human_player = PLAYER;

    // Coup gagnant immédiat
    for (int col = 0; col < COLS; col++) {                                        // On parcours toutes les colonnes
        if (isValidMove(board, col)) {                                            // Pour chaque colonne, on vérifie si on peut y jouer (colonne pas pleine)
            makeMove(board, col, ai_player);                                      // L'IA simule un coup dans la colone
            if (checkWin(board, ai_player)) {                                     // On vérifie si l'IA a gagné immédiatement
                undoMove(board, col);                                             // Si oui, on annule le coup car on ne veut pas le jouer maintenant
                return col;                                                       // On retourne la colonne gagnante
            }
            undoMove(board, col);                                                 // Si ce n'était pas un coup gagnant, on annule quand même le coup pour ne pas modifier le plateau
        }
    }

    // Bloquer joueur s'il peut gagner au prochain coup
    for (int col = 0; col < COLS; col++) {                                       // On parcours toutes les colonnes
        if (isValidMove(board, col)) {                                           // Pour chaque colonne, on vérifie si on peut y jouer (colonne pas pleine)
            makeMove(board, col, human_player);                                  // On simule un coup pour le joueur humain
            if (checkWin(board, human_player)) {                                 // Si ce coup fait gagner le joueur humain
                undoMove(board, col);
                return col;                                                      // On retourne la colonne pour la bloquer immédiatement
            }
            undoMove(board, col);                                                // Sinon on nettoie le coup simulé
        }
    }

    // Minimax
    int bestScore = INT_MIN; // On initialise le score max à une valeur très faible
    int bestCol = -1; // On initialise la colonne à -1 (valeur invalide)

    for (int col = 0; col < COLS; col++) {                                       // On parcours toutes les colonnes
        if (isValidMove(board, col)) {                                           // Pour chaque colonne, on vérifie si on peut y jouer (colonne pas pleine)
            makeMove(board, col, ai_player);                                     // On simule un coup pour l'IA
            int score = minimax(board, profondeur - 1, 0, ai_player, human_player);   // On appelle minimax pour estimer la valeur stratégique de ce coup  | profondeur - 1 car on a déjà joué un coup | 0 car c'est le tour de l'humain
            undoMove(board, col);                                                // On annule le coup simulé
            if (score > bestScore) {                                             // Si le score retourné est supérieur au meilleur score, on le garde en mémoire
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;                                                              // On retourne la colonne avec le score le plus élevé
}