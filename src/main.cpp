#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include "move.cpp" 
#include "ai.cpp"   

// ──────────────────────────────
// SETTINGS
// ──────────────────────────────
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 100;
const int PIECE_WIDTH = 150; 
const int PIECE_HEIGHT = 150;

// ──────────────────────────────
// GLOBAL VARIABLES
// ──────────────────────────────
bool whiteTurn = true;
bool WhiteKingMoved = false, BlackKingMoved = false;
bool WhiteKingsideRookMoved = false, WhiteQueensideRookMoved = false;
bool BlackKingsideRookMoved = false, BlackQueensideRookMoved = false;

bool isDragging = false;
int selectedPiece = -1;
int startCol, startRow;
SDL_Point mouseOffset;
std::vector<std::pair<int, int>> legalMoves;

// UI GLOBALS
Move lastMove = {-1, -1, -1, -1}; // Tracks last move for highlighting
bool isGameOver = false;
std::string gameOverMsg = "";

// AI GLOBALS
bool aiHasMoved = false;
Uint32 lastAITime = 0;
const Uint32 AI_DELAY = 100; 

// BOARD STATE
int board[8][8] = {
    {-1, -2, -3, -4, -5, -3, -2, -1}, 
    {-6, -6, -6, -6, -6, -6, -6, -6}, 
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 6,  6,  6,  6,  6,  6,  6,  6}, 
    { 1,  2,  3,  4,  5,  3,  2,  1}  
};

// ──────────────────────────────
// HELPERS 
// ──────────────────────────────
std::string toChessNotation(int col, int row) {
    char file = 'a' + col;
    char rank = '8' - row;
    return std::string(1, file) + rank;
}

char pieceToSymbol(int piece) {
    switch (abs(piece)) {
        case 1: return 'R'; case 2: return 'N'; case 3: return 'B'; 
        case 4: return 'Q'; case 5: return 'K'; case 6: return ' '; 
        default: return '?';
    }
}

// ──────────────────────────────
// UI FEATURES
// ──────────────────────────────
void checkGameOver(bool isWhiteTurn) {
    std::vector<Move> moves = generateAllMoves(board, isWhiteTurn);
    if (moves.empty()) {
        int kingVal = isWhiteTurn ? 5 : -5;
        int kingRow = -1, kingCol = -1;
        for(int r=0; r<8; r++) {
            for(int c=0; c<8; c++) {
                if(board[r][c] == kingVal) { kingRow=r; kingCol=c; }
            }
        }
        bool inCheck = isSquareAttacked(kingRow, kingCol, !isWhiteTurn, board);
        isGameOver = true;
        gameOverMsg = inCheck ? (isWhiteTurn ? "Checkmate! Black Wins." : "Checkmate! White Wins.") : "Stalemate! Draw.";
    }
}

void renderHighlights(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Yellow Highlight for Last Move
    if (lastMove.fromRow != -1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); 
        SDL_Rect src = {lastMove.fromCol * SQUARE_SIZE, lastMove.fromRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
        SDL_Rect dst = {lastMove.toCol * SQUARE_SIZE, lastMove.toRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
        SDL_RenderFillRect(renderer, &src);
        SDL_RenderFillRect(renderer, &dst);
    }
    // Green Highlight for Dragging
    if (isDragging) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
        SDL_Rect rect = {startCol * SQUARE_SIZE, startRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void renderBoard(SDL_Renderer* renderer, SDL_Texture* boardTexture) {
    SDL_Rect src = {0, 0, 800, 800};
    SDL_Rect dst = {0, 0, BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE};
    SDL_RenderCopy(renderer, boardTexture, &src, &dst);
}

void renderPieces(SDL_Renderer* renderer, SDL_Texture* piecesTexture) {
    SDL_Rect src = {0, 0, PIECE_WIDTH, PIECE_HEIGHT};
    SDL_Rect dst = {0, 0, SQUARE_SIZE, SQUARE_SIZE};

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(board[i][j] != 0) {
                if (isDragging && i == startRow && j == startCol) continue;
                src.x = (abs(board[i][j]) - 1) * PIECE_WIDTH;
                src.y = (board[i][j] > 0) ? PIECE_HEIGHT : 0; 
                dst.x = j * SQUARE_SIZE;
                dst.y = i * SQUARE_SIZE;
                SDL_RenderCopy(renderer, piecesTexture, &src, &dst);
            }
        }
    }
    if(isDragging && selectedPiece != -1) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        src.x = (abs(selectedPiece)-1)* PIECE_WIDTH;
        src.y = (selectedPiece > 0) ? PIECE_HEIGHT : 0;
        dst.x = mx - mouseOffset.x;
        dst.y = my - mouseOffset.y;
        SDL_RenderCopy(renderer, piecesTexture, &src, &dst);
    }
}


void handleEvents(SDL_Event &e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (!whiteTurn) return;
        int col = e.button.x / SQUARE_SIZE;
        int row = e.button.y / SQUARE_SIZE;

        if (board[row][col] != 0) {
            if (whiteTurn && board[row][col] < 0) return;
            isDragging = true;
            selectedPiece = board[row][col];
            startCol = col; startRow = row;
            legalMoves = getLegalMoves(row, col, board);
            mouseOffset.x = e.button.x - (col * SQUARE_SIZE);
            mouseOffset.y = e.button.y - (row * SQUARE_SIZE);
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && isDragging) {
        int col = e.button.x / SQUARE_SIZE;
        int row = e.button.y / SQUARE_SIZE;
        bool isLegal = false;
        for (const auto &m : legalMoves) if (m.first == row && m.second == col) isLegal = true;

        if (isLegal) {
            // CASTLING & FLAGS
            if(abs(selectedPiece) == 5) { if(selectedPiece>0) WhiteKingMoved=true; else BlackKingMoved=true; }
            if(abs(selectedPiece) == 1) {
                if(startRow==7 && startCol==0) WhiteQueensideRookMoved=true;
                if(startRow==7 && startCol==7) WhiteKingsideRookMoved=true;
                if(startRow==0 && startCol==0) BlackQueensideRookMoved=true;
                if(startRow==0 && startCol==7) BlackKingsideRookMoved=true;
            }
            // Move Rook for Castling
            if (abs(selectedPiece)==5 && abs(col-startCol)==2) {
                if (col==6) { board[row][5] = board[row][7]; board[row][7]=0; } // KingSide
                if (col==2) { board[row][3] = board[row][0]; board[row][0]=0; } // QueenSide
            }

            // EXECUTE MOVE
            board[row][col] = selectedPiece;
            board[startRow][startCol] = 0;
            
            // UPDATE UI & LOGIC
            lastMove = {startRow, startCol, row, col};
            whiteTurn = false;
            std::cout << "Move: " << pieceToSymbol(selectedPiece) << toChessNotation(col, row) << std::endl;

            lastAITime = SDL_GetTicks(); 
            
            checkGameOver(false);
        }
        isDragging = false;
        selectedPiece = -1;
    }
}

// ──────────────────────────────
// MAIN LOOP
// ──────────────────────────────
int SDL_main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    SDL_Window* window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BOARD_SIZE*SQUARE_SIZE, BOARD_SIZE*SQUARE_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* bs = SDL_LoadBMP("images/board.bmp");
    SDL_Texture* bTex = SDL_CreateTextureFromSurface(renderer, bs);
    SDL_FreeSurface(bs);

    SDL_Surface* ps = SDL_LoadBMP("images/spriteSheet.bmp");
    SDL_Texture* pTex = SDL_CreateTextureFromSurface(renderer, ps);
    SDL_FreeSurface(ps);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            handleEvents(e); // Calling the local function
        }

        // AI TURN
       if (!whiteTurn && !aiHasMoved) {
            Uint32 now = SDL_GetTicks();
            if (now - lastAITime > AI_DELAY) {
                std::cout << "AI is thinking..." << std::endl;
                Move aiMove = getBestMove(board);
                
                if (aiMove.fromRow != -1) {
                    int piece = board[aiMove.fromRow][aiMove.fromCol];

                    if (abs(piece) == 5 && abs(aiMove.toCol - aiMove.fromCol) == 2) {
                        int row = aiMove.toRow;
                        // Kingside (g-file, col 6)
                        if (aiMove.toCol == 6) { 
                            board[row][5] = board[row][7]; 
                            board[row][7] = 0; 
                        }
                        // Queenside (c-file, col 2)
                        if (aiMove.toCol == 2) { 
                            board[row][3] = board[row][0]; 
                            board[row][0] = 0; 
                        }
                    }

                    if(abs(piece) == 5) { if(piece > 0) WhiteKingMoved = true; else BlackKingMoved = true; }
                    if(abs(piece) == 1) {
                         if(aiMove.fromRow == 0 && aiMove.fromCol == 0) BlackQueensideRookMoved = true;
                         if(aiMove.fromRow == 0 && aiMove.fromCol == 7) BlackKingsideRookMoved = true;
                         // (White rooks technically don't need updates here since AI is black, but good for completeness)
                    }

                    // Normal Move Execution
                    board[aiMove.toRow][aiMove.toCol] = piece;
                    board[aiMove.fromRow][aiMove.fromCol] = 0;

                    std::cout << "AI Moved!" << std::endl;
                    lastMove = {aiMove.fromRow, aiMove.fromCol, aiMove.toRow, aiMove.toCol};
                    whiteTurn = true;
                    aiHasMoved = true;
                    lastAITime = now;
                    checkGameOver(true);
                } else {
                    checkGameOver(false);
                    whiteTurn = true; 
                }
            }
        }
        if (whiteTurn) aiHasMoved = false;

        // RENDER
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        renderBoard(renderer, bTex);
        renderHighlights(renderer);
        renderPieces(renderer, pTex);
        SDL_RenderPresent(renderer);
        if (isGameOver) {
            SDL_Delay(100); // Wait 0.1s so the eye catches the move
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", gameOverMsg.c_str(), window);
            isGameOver = false; // Reset so it doesn't pop up infinitely
            //  set quit = true; here if you want to close the game
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(pTex);
    SDL_DestroyTexture(bTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}