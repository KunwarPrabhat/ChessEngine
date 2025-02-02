#include <SDL.h>
#include <iostream>
#include <string>

// Chessboard dimensions
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 100;  // Size of each square in pixels

// Function to render the board (background)
void renderBoard(SDL_Renderer* renderer, SDL_Texture* boardTexture) {
    SDL_Rect srcRect = {0, 0, 800, 800};  // Define the entire board image
    SDL_Rect destRect = {0, 0, BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE};  // Destination rectangle for rendering

    SDL_RenderCopy(renderer, boardTexture, &srcRect, &destRect);  // Render the board texture
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
    SDL_Surface* boardSurface = SDL_LoadBMP("images/board0.bmp");  // Use .bmp or .png with SDL_image
    if (boardSurface == nullptr) {
        std::cerr << "Failed to load board image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_Texture* boardTexture = SDL_CreateTextureFromSurface(renderer, boardSurface);
    SDL_FreeSurface(boardSurface);

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

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Clean up and close the program
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
