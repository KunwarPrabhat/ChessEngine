#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
// ──────────────────────────────
// Global Variables
// ──────────────────────────────
extern bool WhiteKingMoved;
extern bool BlackKingMoved;
extern bool WhiteKingsideRookMoved;
extern bool WhiteQueensideRookMoved;
extern bool BlackKingsideRookMoved;
extern bool BlackQueensideRookMoved;

// ───────────────────────────────────────────
// Helper function for check detections
// ───────────────────────────────────────────
bool isSquareAttacked(int targetRow, int targetCol, bool attackerIsWhite, int board[8][8]) {
    // 1. Check Pawn Attacks (Pawn direction is inverted for detection)
    int pawnDir = attackerIsWhite ? -1 : 1; 
    if (targetRow - pawnDir >= 0 && targetRow - pawnDir < 8) {
        if (targetCol - 1 >= 0) {
            int p = board[targetRow - pawnDir][targetCol - 1];
            if ((attackerIsWhite && p == 6) || (!attackerIsWhite && p == -6)) return true;
        }
        if (targetCol + 1 < 8) {
            int p = board[targetRow - pawnDir][targetCol + 1];
            if ((attackerIsWhite && p == 6) || (!attackerIsWhite && p == -6)) return true;
        }
    }

    // 2. Check Knight Attacks
    int kMoves[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
    for (auto &m : kMoves) {
        int r = targetRow + m[0], c = targetCol + m[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            int p = board[r][c];
            if ((attackerIsWhite && p == 2) || (!attackerIsWhite && p == -2)) return true;
        }
    }

    // 3. Check Sliding Attacks (Rook, Bishop, Queen)
    int dirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1}, {1,1},{1,-1},{-1,1},{-1,-1}};
    for (int d = 0; d < 8; d++) {
        for (int i = 1; i < 8; i++) {
            int r = targetRow + dirs[d][0] * i;
            int c = targetCol + dirs[d][1] * i;
            if (r < 0 || r >= 8 || c < 0 || c >= 8) break;
            
            int p = board[r][c];
            if (p != 0) { // Hit a piece
                // Orthogonal (Rook/Queen)
                if (d < 4) { 
                    if (attackerIsWhite && (p == 1 || p == 4)) return true;
                    if (!attackerIsWhite && (p == -1 || p == -4)) return true;
                }
                // Diagonal (Bishop/Queen)
                else { 
                    if (attackerIsWhite && (p == 3 || p == 4)) return true;
                    if (!attackerIsWhite && (p == -3 || p == -4)) return true;
                }
                // Also check for King (1 square away)
                if (i == 1) {
                    if ((attackerIsWhite && p == 5) || (!attackerIsWhite && p == -5)) return true;
                }
                break; // Blocked
            }
        }
    }
    return false;
}
// ──────────────────────────────
// Move Generator: Pawn
// ──────────────────────────────
void getPawnMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{
    int piece = board[row][col];
    if (piece == 0)
        return; // No piece here

    int direction = (piece > 0) ? -1 : 1; // White up, Black down

    // Forward 1
    if ((row + direction) >= 0 && (row + direction) < 8 && board[row + direction][col] == 0)
    {
        moves.push_back({row + direction, col});
    }

    // Forward 2 from initial position
    if ((piece > 0 && row == 6) || (piece < 0 && row == 1))
    {
        if ((row + 2 * direction) >= 0 && (row + 2 * direction) < 8 &&
            board[row + direction][col] == 0 &&
            board[row + 2 * direction][col] == 0)
        {
            moves.push_back({row + 2 * direction, col});
        }
    }

    // Diagonal captures (left)
    if (col > 0 && (row + direction) >= 0 && (row + direction) < 8)
    {
        int targetPiece = board[row + direction][col - 1];
        if (targetPiece != 0 && (targetPiece * piece < 0))
        { // Enemy piece
            moves.push_back({row + direction, col - 1});
        }
    }

    // Diagonal captures (right)
    if (col < 7 && (row + direction) >= 0 && (row + direction) < 8)
    {
        int targetPiece = board[row + direction][col + 1];
        if (targetPiece != 0 && (targetPiece * piece < 0))
        { // Enemy piece
            moves.push_back({row + direction, col + 1});
        }
    }
}
// ──────────────────────────────
// Move Generator: Knight
// ──────────────────────────────
void getKnightMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{
    int piece = board[row][col];
    int knightMoves[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
    for (int i = 0; i < 8; i++)
    {
        int newRow = row + knightMoves[i][0];
        int newCol = col + knightMoves[i][1];
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0)
            {
                // empty
                moves.push_back({newRow, newCol});
            }
            else if ((board[newRow][newCol] < 0 && piece > 0) || (board[newRow][newCol] > 0 && piece < 0))
            {
                // occupied
                moves.push_back({newRow, newCol});
            }
        }
    }
}
// ──────────────────────────────
// Move Generator: Bishop
// ──────────────────────────────
void getBishopMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{
    int piece = board[row][col];
    if (piece == 0)
        return;

    // Four diagonal directions: NE, NW, SE, SW
    // int directions[4][2] = {{1,1}, {1,-1}, {-1,1}, {-1,-1}}; //will use later during code optimization.

    for (int i = 1; i < 8; i++)
    {
        int newRow = row + i;
        int newCol = col + i;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0)
            {
                // empty
                moves.push_back({newRow, newCol});
            }
            else if ((board[newRow][newCol] < 0 && piece > 0) || (board[newRow][newCol] > 0 && piece < 0))
            {
                moves.push_back({newRow, newCol});
                break; // this is important so that the bishop doesn't jump over other pieces.
            }
            else
            {
                break; // friendly piece blocks
            }
        }
    }
    for (int i = 1; i < 8; i++)
    {
        int newRow = row + i;
        int newCol = col - i;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0)
            {
                // empty
                moves.push_back({newRow, newCol});
            }
            else if ((board[newRow][newCol] < 0 && piece > 0) || (board[newRow][newCol] > 0 && piece < 0))
            {
                moves.push_back({newRow, newCol});
                break; // this is important so that the bishop doesn't jump over other pieces.
            }
            else
            {
                break; // friendly piece blocks
            }
        }
    }
    for (int i = 1; i < 8; i++)
    {
        int newRow = row - i;
        int newCol = col + i;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0)
            {
                // empty
                moves.push_back({newRow, newCol});
            }
            else if ((board[newRow][newCol] < 0 && piece > 0) || (board[newRow][newCol] > 0 && piece < 0))
            {
                moves.push_back({newRow, newCol});
                break; // this is important so that the bishop doesn't jump over other pieces.
            }
            else
            {
                break; // friendly piece blocks
            }
        }
    }
    for (int i = 1; i < 8; i++)
    {
        int newRow = row - i;
        int newCol = col - i;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0)
            {
                // empty
                moves.push_back({newRow, newCol});
            }
            else if ((board[newRow][newCol] < 0 && piece > 0) || (board[newRow][newCol] > 0 && piece < 0))
            {
                moves.push_back({newRow, newCol});
                break; // this is important so that the bishop doesn't jump over other pieces.
            }
            else
            {
                break; // friendly piece blocks
            }
        }
    }
}
// ──────────────────────────────
// Move Generator: Rook
// ──────────────────────────────
void getRookMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{
    int piece = board[row][col];
    if (piece == 0)
        return;
    int directions[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    for (int d = 0; d < 4; d++)
    {
        int dx = directions[d][0];
        int dy = directions[d][1];
        for (int i = 1; i < 8; i++)
        {
            int newRow = row + dx * i;
            int newCol = col + dy * i;

            if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8)
                break;
            if (board[newRow][newCol] == 0)
            {
                moves.push_back({newRow, newCol});
            }
            else if ((piece > 0 && board[newRow][newCol] < 0) || (piece < 0 && board[newRow][newCol] > 0))
            {
                moves.push_back({newRow, newCol});
                break;
            }
            else
            {
                break;
            }
        }
    }
}
// ──────────────────────────────
// Move Generator: queen
// ──────────────────────────────
void getQueenMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{
    int piece = board[row][col];
    int directions[8][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    if (piece == 0)
        return;

    for (int d = 0; d < 8; d++)
    {
        int dx = directions[d][0];
        int dy = directions[d][1];
        for (int i = 1; i < 8; i++)
        {
            int newRow = row + dx * i;
            int newCol = col + dy * i;

            if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8)
                break;
            if (board[newRow][newCol] == 0)
            {
                moves.push_back({newRow, newCol});
            }
            else if ((piece > 0 && board[newRow][newCol] < 0) || (piece < 0 && board[newRow][newCol] > 0))
            {
                moves.push_back({newRow, newCol});
                break;
            }
            else
            {
                break;
            }
        }
    }
}
// ──────────────────────────────
// Move Generator: King
// ──────────────────────────────
void getKingMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>> &moves)
{

    int piece = board[row][col];
    int directions[8][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    if (piece == 0)
        return;
    for (int d = 0; d < 8; d++)
    {
        int newRow = row + directions[d][0];
        int newCol = col + directions[d][1];
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (board[newRow][newCol] == 0 ||
                (piece > 0 && board[newRow][newCol] < 0) ||
                (piece < 0 && board[newRow][newCol] > 0))
            {
                moves.push_back({newRow, newCol});
            }
        }
    }
    // ──────────────────────────────
    // Castling logic for white king
    //  ──────────────────────────────
    if(abs(piece) == 5){ //for identifying the king.
        if (piece > 0 && !WhiteKingMoved && row == 7 && col == 4)
        {
            // kingside
            if (!WhiteKingsideRookMoved && 
                board[7][5] == 0 && 
                board[7][6] == 0)
            {
                moves.push_back({7, 6});
            }
            // queenside
            if (!WhiteQueensideRookMoved && 
                board[7][3] == 0 && 
                board[7][2] == 0 && 
                board[7][1] == 0)
            {
                moves.push_back({7, 2});
            }
        }
        // ──────────────────────────────
        // Castling logic for Black king
        // ──────────────────────────────
        else if(piece < 0 && !BlackKingMoved && row == 0 && col == 4)
        {
            // kingside
            if (!BlackKingsideRookMoved && 
                board[0][5] == 0 && 
                board[0][6] == 0)
            {
                moves.push_back({0, 6});
            }
            // queenside
            if (!BlackQueensideRookMoved && 
                board[0][3] == 0 && 
                board[0][2] == 0 && 
                board[0][1] == 0)
            {
                moves.push_back({0, 2});
            }
        }
    }
}
// ──────────────────────────────
// Dispatcher: Legal Moves
// ──────────────────────────────
std::vector<std::pair<int, int>> getLegalMoves(int row, int col, int board[8][8])
{
    std::vector<std::pair<int, int>> rawMoves; // Temporary list
    int piece = board[row][col];
    if (piece == 0) return rawMoves;

    // 1. Get Geometry-based moves (Same as before, but into rawMoves)
    switch (abs(piece))
    {
    case 1: getRookMoves(row, col, board, rawMoves); break;
    case 2: getKnightMoves(row, col, board, rawMoves); break;
    case 3: getBishopMoves(row, col, board, rawMoves); break;
    case 4: getQueenMoves(row, col, board, rawMoves); break;
    case 5: getKingMoves(row, col, board, rawMoves); break;
    case 6: getPawnMoves(row, col, board, rawMoves); break;
    }

    // 2. Filter: Remove moves that leave the King in check
    std::vector<std::pair<int, int>> finalMoves;
    bool isWhite = (piece > 0);

    for (const auto &move : rawMoves)
    {
        int targetRow = move.first;
        int targetCol = move.second;
        
        // A. Make the move temporarily
        int originalTargetPiece = board[targetRow][targetCol];
        board[targetRow][targetCol] = piece;
        board[row][col] = 0;

        // B. Find my King's location
        int kingRow = -1, kingCol = -1;
        for(int r=0; r<8; r++) {
            for(int c=0; c<8; c++) {
                if(board[r][c] == (isWhite ? 5 : -5)) {
                    kingRow = r; kingCol = c;
                }
            }
        }

        // C. Check if King is attacked (Pass !isWhite because we check if ENEMY attacks us)
        if (kingRow != -1 && !isSquareAttacked(kingRow, kingCol, !isWhite, board)) {
            finalMoves.push_back(move);
        }

        // D. Undo the move
        board[row][col] = piece;
        board[targetRow][targetCol] = originalTargetPiece;
    }

    return finalMoves;
}

