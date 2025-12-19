#include <vector>
#include <limits>
#include <utility>
#include <cstdlib>
#include <algorithm> 

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    int score; 
};

// Forward Declarations
std::vector<std::pair<int,int>> getLegalMoves(int row, int col, int board[8][8]);
bool isSquareAttacked(int targetRow, int targetCol, bool attackerIsWhite, int board[8][8]);

// ──────────────────────────────────────────────
// GLOBALS & CONSTANTS
// ──────────────────────────────────────────────
// Piece Values: 0, Rook, Knight, Bishop, Queen, King, Pawn
const int PIECE_VALUES[7] = {0, 500, 320, 330, 900, 20000, 100};

// PIECE-SQUARE TABLES (Bonus Points for Position)
int pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

int knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

int bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

int rookTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

int queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

int kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

// ──────────────────────────────────────────────
// OPTIMIZATION: Move Ordering
// ──────────────────────────────────────────────
// Assigns a score to each move so we can sort them.
// High Score = Check this move first (Optimizes Alpha-Beta)
void orderMoves(std::vector<Move>& moves, int board[8][8]) {
    for (auto& move : moves) {
        int moveScore = 0;
        int victim = abs(board[move.toRow][move.toCol]);
        int attacker = abs(board[move.fromRow][move.fromCol]);

        // 1. Prioritize Captures (MVV-LVA: Most Valuable Victim - Least Valuable Attacker)
        if (victim != 0) {
            moveScore = 10 * PIECE_VALUES[victim] - PIECE_VALUES[attacker];
        }

        move.score = moveScore;
    }

    // Sort moves: Highest score first
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

// ──────────────────────────────────────────────
// EVALUATION FUNCTION
// ──────────────────────────────────────────────
int evaluateBoard(int board[8][8]) {
    int score = 0;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int piece = board[r][c];
            if (piece == 0) continue;

            int absPiece = abs(piece);
            int baseValue = PIECE_VALUES[absPiece];
            int positionBonus = 0;

            int tableRow = (piece > 0) ? r : (7 - r);

            switch(absPiece) {
                case 1: positionBonus = rookTable[tableRow][c]; break;
                case 2: positionBonus = knightTable[tableRow][c]; break;
                case 3: positionBonus = bishopTable[tableRow][c]; break;
                case 4: positionBonus = queenTable[tableRow][c]; break;
                case 5: positionBonus = kingTable[tableRow][c]; break;
                case 6: positionBonus = pawnTable[tableRow][c]; break;
            }

            if (piece > 0) score += (baseValue + positionBonus);
            else score -= (baseValue + positionBonus);
        }
    }
    return score;
}

std::vector<Move> generateAllMoves(int board[8][8], bool whiteTurn) {
    std::vector<Move> moves;
    // Reserve memory to avoid reallocations (Optimization)
    moves.reserve(30); 

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int piece = board[r][c];
            if (piece == 0) continue;
            if (whiteTurn && piece < 0) continue;
            if (!whiteTurn && piece > 0) continue;

            std::vector<std::pair<int, int>> legal = getLegalMoves(r, c, board);
            for (auto &m : legal) {
                moves.push_back({r, c, m.first, m.second, 0});
            }
        }
    }
    return moves;
}

// ──────────────────────────────────────────────
// MINIMAX WITH ALPHA-BETA & MOVE ORDERING
// ──────────────────────────────────────────────
int minimax(int board[8][8], int depth, bool maximizing, int alpha, int beta) {
    std::vector<Move> moves = generateAllMoves(board, maximizing);

    if (moves.empty()) {
        int kingRow = -1, kingCol = -1;
        int kingVal = maximizing ? 5 : -5;
        for(int r=0; r<8; r++) {
            for(int c=0; c<8; c++) {
                if(board[r][c] == kingVal) { kingRow=r; kingCol=c; }
            }
        }
        if (isSquareAttacked(kingRow, kingCol, !maximizing, board)) {
            return maximizing ? -9999999 - depth : 9999999 + depth;
        }
        return 0; // Stalemate
    }
    
    if (depth == 0) return evaluateBoard(board);

    // NEW: Order moves to make Alpha-Beta effective
    orderMoves(moves, board);

    if (maximizing) { // White
        int maxEval = -99999999;
        for (auto &m : moves) {
            int captured = board[m.toRow][m.toCol];
            int movingPiece = board[m.fromRow][m.fromCol];

            board[m.toRow][m.toCol] = movingPiece;
            board[m.fromRow][m.fromCol] = 0;

            int eval = minimax(board, depth - 1, false, alpha, beta);

            board[m.fromRow][m.fromCol] = movingPiece;
            board[m.toRow][m.toCol] = captured;

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Pruning
        }
        return maxEval;
    } else { // Black
        int minEval = 99999999;
        for (auto &m : moves) {
            int captured = board[m.toRow][m.toCol];
            int movingPiece = board[m.fromRow][m.fromCol];

            board[m.toRow][m.toCol] = movingPiece;
            board[m.fromRow][m.fromCol] = 0;

            int eval = minimax(board, depth - 1, true, alpha, beta);

            board[m.fromRow][m.fromCol] = movingPiece;
            board[m.toRow][m.toCol] = captured;

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Pruning
        }
        return minEval;
    }
}

Move getBestMove(int board[8][8]) {
    int bestScore = 99999999;
    Move bestMove = {-1, -1, -1, -1, 0};
    
    std::vector<Move> moves = generateAllMoves(board, false);
    
    // NEW: Order moves at the root too
    orderMoves(moves, board);

    for (auto &m : moves) {
        int captured = board[m.toRow][m.toCol];
        int movingPiece = board[m.fromRow][m.fromCol];

        board[m.toRow][m.toCol] = movingPiece;
        board[m.fromRow][m.fromCol] = 0;

        int score = minimax(board, 4, true, -99999999, 99999999);

        board[m.fromRow][m.fromCol] = movingPiece;
        board[m.toRow][m.toCol] = captured;

        if (score < bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }
    return bestMove;
}