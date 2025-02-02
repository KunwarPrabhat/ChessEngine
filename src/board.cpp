#include<iostream>

char board[8][8];

void initBoard() {
    std::string pieces = "RNBQKBNR";
    for(int i = 0; i < 8; i++){
        board[0][i] = tolower(pieces[i]);
        board[1][i] = 'p';
        board[6][i] = 'P';
        board[7][i] = pieces[i];
    }
    for(int i = 2; i<6; i++){
        for(int j = 0; j<8; j++)
            board[i][j] = '.';
    }
}
void printBoard(){
    for(int i = 0; i< 8; i++){
        std::cout<<(8-i) <<" "; //row
        for (int j = 0; j < 8; j++){
            std::cout<< board[i][j]<<" ";
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h\n";//column
}