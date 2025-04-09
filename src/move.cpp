#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

// ──────────────────────────────
// Global Variables
// ──────────────────────────────
bool WhiteKingMoved = false, BlackKingMoved = false;
bool WhiteKingsideRookMoved = false, WhiteQueensideRookMoved = false;
bool BlackKingsideRookMoved = false, BlackQueensideRookMoved = false;
bool isDragging = false;
int selectedPiece = -1;
int startCol, startRow;
SDL_Point mouseOffset;
std::vector<std::pair<int, int>> legalMoves;

// ──────────────────────────────
// Utility: Convert to Chess Notation
// ──────────────────────────────
std::string toChessNotation(int col, int row)
{
    char file = 'a' + col;
    char rank = '8' - row;
    return std::string(1, file) + rank;
}

// ──────────────────────────────
// Utility: Convert Piece to Symbol
// ──────────────────────────────
char pieceToSymbol(int piece)
{
    switch (abs(piece))
    {
    case 1:
        return 'R'; // Rook
    case 2:
        return 'N'; // Knight
    case 3:
        return 'B'; // Bishop
    case 4:
        return 'Q'; // Queen
    case 5:
        return 'K'; // King
    case 6:
        return ' '; // Pawn
    default:
        return '?';
    }
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
    std::vector<std::pair<int, int>> moves;
    int piece = board[row][col];
    if (piece == 0)
        return moves;

    std::cout << "Getting moves for piece at " << row << "," << col
              << " (value: " << piece << ")" << std::endl;

    switch (abs(piece))
    {
    case 1:
        getRookMoves(row, col, board, moves);
        break;
    case 2:
        getKnightMoves(row, col, board, moves);
        break;
    case 3:
        getBishopMoves(row, col, board, moves);
        break;
    case 4:
        getQueenMoves(row, col, board, moves);
        break;
    case 5:
        getKingMoves(row, col, board, moves);
        break;
    case 6:
        getPawnMoves(row, col, board, moves);
        break;
    default:
        break;
    }

    std::cout << "Found " << moves.size() << " legal moves:" << std::endl;
    for (const auto &move : moves)
    {
        std::cout << "  -> " << move.first << "," << move.second << std::endl;
    }

    return moves;
}

// ──────────────────────────────
// Event Handler: Drag & Drop
// ──────────────────────────────
void handleEvents(SDL_Event &e, int board[8][8])
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        int col = mouseX / 100;
        int row = mouseY / 100;

        if (board[row][col] != 0)
        {
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

    if (e.type == SDL_MOUSEBUTTONUP && isDragging)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        int col = mouseX / 100;
        int row = mouseY / 100;

        bool isLegalMove = false;
        for (const auto &move : legalMoves)
        {
            if (move.first == row && move.second == col)
            {
                isLegalMove = true;
                break;
            }
        }

        if (isLegalMove)
        {
            if(abs(selectedPiece) == 5){ //5 is for the king
                if(selectedPiece > 0) WhiteKingMoved = true;
                else BlackKingMoved = true;
            }
            else if (abs(selectedPiece) == 1 ){ //1 is for the Rook
                if(selectedPiece > 0){
                    if(startRow == 7 && startCol == 0){ 
                        WhiteQueensideRookMoved = true;
                        std::cout << "White queenside (left) rook has moved" << std::endl; //debug statements
                    }
                    if(startRow == 7 && startCol == 7){ 
                        WhiteKingsideRookMoved = true; 
                        std::cout << "White kingside (right) rook has moved" << std::endl; //debug statements
                    }
                    

                } else {
                    if (startRow == 0 && startCol == 0) {
                        BlackQueensideRookMoved = true;
                        std::cout << "Black queenside (left) rook has moved" << std::endl; //debug statements
                    }
                    if (startRow == 0 && startCol == 7) {
                        BlackKingsideRookMoved = true;
                        std::cout << "Black kingside (right) rook has moved" << std::endl; //debug statements
                    }
                }
            }

            // White Kingside Castling {moving the rook to (7,5) when the king is moved to (7,6)}
            if (selectedPiece == 5 && startRow == 7 && startCol == 4 && row == 7 && col == 6)
            {
                board[7][5] = board[7][7];
                board[7][7] = 0;
            }
            // White QueenSide Castling {moving the rook to (7,3) when the king is moved to (7,2)}
            else if (selectedPiece == 5 && startRow == 7 && startCol == 4 && row == 7 && col == 2)
            {
                board[7][3] = board[7][0]; // Move rook from a1 to d1
                board[7][0] = 0;
            }
            // Black Kingside Castling {moving the rook to (0,5) when the king is moved to (0,6)}
            else if (selectedPiece == -5 && startRow == 0 && startCol == 4 && row == 0 && col == 6)
            {
                board[0][5] = board[0][7];
                board[0][7] = 0;
            }
            // Black QueenSide Castling {moving the rook to (0,3) when the king is moved to (0,2)}
            else if (selectedPiece == -5 && startRow == 0 && startCol == 4 && row == 0 && col == 2)
            {
                board[0][3] = board[0][0]; // Move rook from a1 to d1
                board[0][0] = 0;
            }
            board[row][col] = selectedPiece;
            std::string move = pieceToSymbol(selectedPiece) + toChessNotation(col, row);
            std::cout << "Move: " << move << std::endl;
        }
        else
        {
            // Put the piece back to its original square if move is illegal
            board[startRow][startCol] = selectedPiece;
        }

        isDragging = false;
        selectedPiece = -1; // Reset selected piece
    }
}

