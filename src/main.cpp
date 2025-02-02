#include <SDL.h>
#include <iostream>
#include <string>

// Screen dimensions for the window
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// This function is responsible for rendering the chessboard
void renderBoard(SDL_Renderer* renderer, SDL_Texture* boardTexture) {
    // Define the source and destination rectangles for rendering the board
    SDL_Rect srcRect = {0, 0, 800, 800};  // Source rectangle (whole image)
    SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};  // Destination rectangle (on-screen position)
    
    // Render the chessboard image onto the screen
    SDL_RenderCopy(renderer, boardTexture, &srcRect, &destRect);
}

// Main function for the program
int SDL_main(int argc, char* argv[]) {
    // Initialize SDL for video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;  // Return with error code if initialization fails
    }

    // Create a window for the chessboard
    SDL_Window* window = SDL_CreateWindow("Chessboard with SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;  // Return with error code if window creation fails
    }

    // Create a renderer to render graphics on the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;  // Return with error code if renderer creation fails
    }

    // Load the chessboard image (this will be the background of the window)
    SDL_Surface* boardSurface = SDL_LoadBMP("images/board.bmp");  // Load the image as surface (use SDL_image for PNG)
    if (boardSurface == nullptr) {
        std::cerr << "Failed to load board image! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;  // Return with error code if image loading fails
    }
    SDL_Texture* boardTexture = SDL_CreateTextureFromSurface(renderer, boardSurface);  // Convert surface to texture
    SDL_FreeSurface(boardSurface);  // Free the surface after conversion

    // Main game loop to keep the window open and refresh
    bool quit = false;
    SDL_Event e;  // Event variable to handle user input and events
    while (!quit) {
        // Handle all incoming events (like closing the window)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {  // If the user clicks the close button
                quit = true;  // Set quit to true to end the loop
            }
        }

        // Clear the screen with a white background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Set color to white
        SDL_RenderClear(renderer);  // Clear the screen

        // Draw the chessboard onto the screen
        renderBoard(renderer, boardTexture);

        // Show everything we've rendered so far
        SDL_RenderPresent(renderer);
    }

    // Clean up and close the program
    SDL_DestroyTexture(boardTexture);  // Free the board texture
    SDL_DestroyRenderer(renderer);  // Destroy the renderer
    SDL_DestroyWindow(window);  // Destroy the window
    SDL_Quit();  // Quit SDL

    return 0;  // Successfully exit the program
}
