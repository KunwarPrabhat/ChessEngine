## An ongoing project on my custom chess engine using c++ and SDL library.
## How to run(for mingw vs code) :-
- Download and run "mingw32-make-3.80.0-3.exe" to default mingw location.
- Open terminal and follow up these commands.
```sh
$ git clone https://github.com/KunwarPrabhat/ChessEngine
$ cd ChessEngine
$ cd src
$ mingw32-make
$ main.exe
```
### So far :-
  - prints a chess board with it's pieces using texture rendering with SDL library.
  - User can drag and drop the pieces over the chess board.
  - pices now follow their legal moves basied on chess rules.

### Things To Add :-
- Implement piece movement logic. {Done}
- Add move validation and legality checks. {Done}
- Implement Kingside & QueenSide Castling. {Done}
- Implement Special move En passant.
- Implement pawn promotion. 
- Introduce AI logic to play moves autonomously.
- Improve the engine’s playing strength over time.
- Optimize performance for speed and efficiency.
