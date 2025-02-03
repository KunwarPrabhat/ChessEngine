#include <SDL.h>
#include <iostream>
#include <string>

// Chessboard dimensions
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 100;  // Size of each square in pixels
const int PIECE_WIDTH = 150;  // Calculated piece width
const int PIECE_HEIGHT = 150;

int board[8][8] = {
    -1, -2, -3, -4, -5, -3, -2, -1,
    -6, -6, -6, -6, -6, -6, -6, -6,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     6,  6,  6,  6,  6,  6,  6,  6,
     1,  2,  3,  4,  5,  3,  2,  1
};
// Function to render the board (background)
void renderBoard(SDL_Renderer* renderer, SDL_Texture* boardTexture) {
    SDL_Rect srcRect = {0, 0, 800, 800};  // Define the entire board image
    SDL_Rect destRect = {0, 0, BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE};  // Destination rectangle for rendering

    SDL_RenderCopy(renderer, boardTexture, &srcRect, &destRect);  // Render the board texture
}
void renderPieces(SDL_Renderer* renderer, SDL_Texture* piecesTexture) {
    SDL_Rect srcRect = {0, 0, PIECE_WIDTH, PIECE_HEIGHT};  
    SDL_Rect destRect = {0, 0, SQUARE_SIZE, SQUARE_SIZE};  // Scale to fit the board squares

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] != 0) {
                srcRect.x = (abs(board[i][j]) - 1) * PIECE_WIDTH;
                srcRect.y = (board[i][j] > 0) ? PIECE_HEIGHT : 0;

                // Scale the piece to fit within the square
                destRect.x = j * SQUARE_SIZE;
                destRect.y = i * SQUARE_SIZE;
                destRect.w = SQUARE_SIZE;  // Scale width to fit the square
                destRect.h = SQUARE_SIZE;  // Scale height to fit the square

                SDL_RenderCopy(renderer, piecesTexture, &srcRect, &destRect);  
            }
        }   
    }
}

// SDL_main - SDL's entry point (instead of the regular main)
int SDL_main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create window with size based on the chessboard (8x8 grid, each square is SQUARE_SIZE)
    SDL_Window* window = SDL_CreateWindow("Chessboard with SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load the chessboard image (replace with the correct path)
    SDL_Surface* boardSurface = SDL_LoadBMP("images/board.bmp");  // Use .bmp or .png with SDL_image
    if (boardSurface == nullptr) {
        std::cerr << "Failed to load board image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_Texture* boardTexture = SDL_CreateTextureFromSurface(renderer, boardSurface);
    SDL_FreeSurface(boardSurface);

    SDL_Surface* piecesSurface = SDL_LoadBMP("images/spriteSheet.bmp");
    if (piecesSurface == nullptr) {
        std::cerr << "Failed to load pieces image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyTexture(boardTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_Texture* piecesTexture = SDL_CreateTextureFromSurface(renderer, piecesSurface);
    SDL_FreeSurface(piecesSurface);
    // Main loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
        SDL_RenderClear(renderer);

        // Render the chessboard (no pieces, just the board)
        renderBoard(renderer, boardTexture);

        // Render the pieces
        renderPieces(renderer, piecesTexture);

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Clean up and close the program
    SDL_DestroyTexture(piecesTexture);
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
