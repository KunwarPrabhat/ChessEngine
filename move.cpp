#include <iostream>

bool isValidMove(std::string move) {
    // Ensure move string is exactly 5 characters (e.g., "e2 e4")
    if (move.length() != 5 || move[2] != ' ') {
        std::cout << "Invalid input format! Use: e2 e4\n";
        return false;
    }

    int fromY = move[0] - 'a';
    int fromX = 8 - (move[1] - '1');
    int toY = move[3] - 'a';
    int toX = 8 - (move[4] - '1');

    if (fromX < 0 || fromX >= 8 || fromY < 0 || fromY >= 8 ||
        toX < 0 || toX >= 8 || toY < 0 || toY >= 8) {
        std::cout << "Move out of bounds!\n";
        return false;
    }

    char piece = board[fromX][fromY];

    if (piece == 'P') { // Simple Pawn Move
        return (toX == fromX - 1 && fromY == toY && board[toX][toY] == '.');
    }

    return false;
}

void makeMove(std::string move) {
    if (move.length() != 5 || move[2] != ' ') {
        std::cout << "Invalid move format!\n";
        return;
    }

    int fromY = move[0] - 'a';
    int fromX = 8 - (move[1] - '1');
    int toY = move[3] - 'a';
    int toX = 8 - (move[4] - '1');

    board[toX][toY] = board[fromX][fromY];
    board[fromX][fromY] = '.';
}
