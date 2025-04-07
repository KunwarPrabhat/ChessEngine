#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

// ──────────────────────────────
// Global Variables
// ──────────────────────────────
bool isDragging = false;
int selectedPiece = -1;
int startCol, startRow;
SDL_Point mouseOffset;
std::vector<std::pair<int, int>> legalMoves;

// ──────────────────────────────
// Utility: Convert to Chess Notation
// ──────────────────────────────
std::string toChessNotation(int col, int row) {
    char file = 'a' + col;
    char rank = '8' - row;
    return std::string(1, file) + rank;
}

// ──────────────────────────────
// Utility: Convert Piece to Symbol
// ──────────────────────────────
char pieceToSymbol(int piece) {
    switch (abs(piece)) {
        case 1: return 'R'; // Rook
        case 2: return 'N'; // Knight
        case 3: return 'B'; // Bishop
        case 4: return 'Q'; // Queen
        case 5: return 'K'; // King
        case 6: return ' '; // Pawn 
        default: return '?';
    }
}

// ──────────────────────────────
// Move Generator: Pawn
// ──────────────────────────────
void getPawnMoves(int row, int col, int board[8][8], std::vector<std::pair<int, int>>& moves) {
    int piece = board[row][col];
    if (piece == 0) return;  // No piece here
    
    int direction = (piece > 0) ? -1 : 1; // White up, Black down

    // Forward 1
    if ((row + direction) >= 0 && (row + direction) < 8 && board[row + direction][col] == 0) {
        moves.push_back({row + direction, col});
    }

    // Forward 2 from initial position
    if ((piece > 0 && row == 6) || (piece < 0 && row == 1)) {
        if ((row + 2 * direction) >= 0 && (row + 2 * direction) < 8 &&
            board[row + direction][col] == 0 && 
            board[row + 2 * direction][col] == 0) {
            moves.push_back({row + 2 * direction, col});
        }
    }

    // Diagonal captures (left)
    if (col > 0 && (row + direction) >= 0 && (row + direction) < 8) {
        int targetPiece = board[row + direction][col - 1];
        if (targetPiece != 0 && (targetPiece * piece < 0)) { // Enemy piece
            moves.push_back({row + direction, col - 1});
        }
    }

    // Diagonal captures (right)
    if (col < 7 && (row + direction) >= 0 && (row + direction) < 8) {
        int targetPiece = board[row + direction][col + 1];
        if (targetPiece != 0 && (targetPiece * piece < 0)) { // Enemy piece
            moves.push_back({row + direction, col + 1});
        }
    }
}
// ──────────────────────────────
// Dispatcher: Legal Moves
// ──────────────────────────────
std::vector<std::pair<int, int>> getLegalMoves(int row, int col, int board[8][8]) {
    std::vector<std::pair<int, int>> moves;
    int piece = board[row][col];
    if (piece == 0) return moves;

    std::cout << "Getting moves for piece at " << row << "," << col 
              << " (value: " << piece << ")" << std::endl;

    switch (abs(piece)) {
        case 6: getPawnMoves(row, col, board, moves); break;
        default: break;
    }

    std::cout << "Found " << moves.size() << " legal moves:" << std::endl;
    for (const auto& move : moves) {
        std::cout << "  -> " << move.first << "," << move.second << std::endl;
    }

    return moves;
}

// ──────────────────────────────
// Event Handler: Drag & Drop
// ──────────────────────────────
void handleEvents(SDL_Event& e, int board[8][8]) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        int col = mouseX / 100;
        int row = mouseY / 100;

        if (board[row][col] != 0) {
            isDragging = true;
            selectedPiece = board[row][col];
            startCol = col;
            startRow = row;
            
            // Calculate legal moves BEFORE modifying the board
            legalMoves = getLegalMoves(row, col, board);
            
            // Now remove the piece for dragging
            board[row][col] = 0;

            mouseOffset.x = mouseX - (col * 100);
            mouseOffset.y = mouseY - (row * 100);
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && isDragging) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        int col = mouseX / 100;
        int row = mouseY / 100;

        bool isLegalMove = false;
        for (const auto& move : legalMoves) {
            if (move.first == row && move.second == col) {
                isLegalMove = true;
                break;
            }
        }

        if (isLegalMove) {
            board[row][col] = selectedPiece;
            std::string move = pieceToSymbol(selectedPiece) + toChessNotation(col, row);
            std::cout << "Move: " << move << std::endl;
        } else {
            // Put the piece back to its original square if move is illegal
            board[startRow][startCol] = selectedPiece;
        }

        isDragging = false;
        selectedPiece = -1; // Reset selected piece
    }
}

