#include <iostream>
#include "board.cpp"
#include "move.cpp"

int main() {
    initBoard();
    printBoard();

    while(true) {
        std::string move;
        std::cout<<"Enter move (e.g, e2, e4): ";
        std::cin>> move;

        if(move == "exit") break;

        if(isValidMove(move)) {
            makeMove(move);
            printBoard();
        }else{
            std::cout<<"Invalid move! Try again. \n";
        }
    }
    return 0;
}