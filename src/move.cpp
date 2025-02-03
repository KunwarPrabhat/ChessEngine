#include <SDL.h>
#include <iostream>
#include <string>

//Global variables
bool isDragging = false;
int selectedPiece = -1;
int startCol, startRow;
SDL_Point mouseOffset; 

//conversion of board coordinates to chess notations.
std::string toChessNotation(int col, int row) {
    char file = 'a' + col;
    char rank = '8' - row;
    return std::string(1, file) + rank;
}

//Maps numeric piece values to chess symbbols
char pieceToSymbol(int piece) {
    switch (abs(piece)){
        case 1: return 'R'; // Rook
        case 2: return 'N'; // Knight
        case 3: return 'B'; // Bishop
        case 4: return 'Q'; // Queen
        case 5: return 'K'; // King
        case 6: return ' '; // Pawn
        default: return '?';
    }
}
//handle mouse events for dragging and dropping 
void handleEvents(SDL_Event& e, int board[8][8]) {
    if(e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;

        int col = mouseX/100; //square size is 100.
        int row = mouseY/100;

        if(board[row][col]!=0){
            isDragging = true;
            selectedPiece = board[row][col];
            startCol = col;
            startRow = row;
            board[row][col] = 0;

            mouseOffset.x = mouseX - (col * 100);
            mouseOffset.y = mouseY - (row * 100);
        }
    }
     if (e.type == SDL_MOUSEBUTTONUP && isDragging) { // Dropping the piece
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        
        int col = mouseX / 100;
        int row = mouseY / 100;

        board[row][col] = selectedPiece; // Place piece on new square
        isDragging = false;

        // Print move in chess notation
        std::string move = pieceToSymbol(selectedPiece) + toChessNotation(col, row);
        std::cout << "Move: " << move << std::endl;
    }
}