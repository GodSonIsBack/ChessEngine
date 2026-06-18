# Chess Engine in C++

A chess engine built completely from scratch in C++17 without the use of any libraries or frameworks.

The engine provides two playable modes:
- It has support for UCI protocol so it can work with any UCI compatible chess GUI.
- It also supports play in the terminal itself.

Optimized heavily with many different search optimizations to make the search at depth 7 fastest achievable.


## Features

### Search Algorithm
- **Negamax with Alpha-Beta Pruning** — Implemented NegaMax instead of the plain minimax with alpha-beta bounds to prune off branches not affecting the results, massively reducing the number of positions the engine has to look at.

- **Iterative Deepening** — Rather than jumping straight to depth 7, the engine searches depths incrementally from depth 1, then depth 2, then depth 3... all the way up to the target depth. Each iteration seeds the transposition table making the search at deeper depths complete faster.

- **Mate Distance Pruning** — Checkmate scores are adjusted by ply distance (`-MATE_VALUE + ply`), so the engine always prefers the fastest mate over a slower one.

### Move Ordering
- **MVV-LVA (Most Valuable Victim – Least Valuable Attacker)** — Sorted captures according to the **MVV-LVA** move ordering technique making the alpha-beta prune more efficient and searching the tree faster.

- **Transposition Table Move Priority** — assigns a massive score bonus (20,000) to the move returned from Transposition Table having it to be searched first, leading to a massive speedup in the engine.

### Transposition Table
- **Incremental Zobrist Hashing** — Every board position gets a unique 64-bit hash using XOR of pre-generated random numbers. The hash updates incrementally on each move (no need to rehash the entire board), making it essentially free.

- **1M Entry Hash Table** — A 2^20 entry table indexed by bitwise masking. Storing the score, depth, best move, and a flag (EXACT / LOWERBOUND / UPPERBOUND) for each position. Prevents the engine from re-evaluating positions it's already seen leading to a massive boost in performance and a Huge ELO boost of the engine.

### Evaluation
- **Material Counting + Piece-Square Tables(PSTs)** — The evaluation is a sum of material values (pawn=100, knight/bishop=300, rook=500, queen=900) and positional bonuses from pre-written **PSTs** encouraging piece mobility.

- **Incremental Evaluation** — Have the `makeMove` / `unmakeMove` functions incrementally update the score at each move optimising the evaluation from `O(N)` to `O(1)`. 

### Board Representation
- **Array-Based (int[64])** — A simple flat array where index 0 = A1 and index 63 = H8. Pieces are stored as enum values.

- **Full FEN Parsing** — `loadFEN` function in `Board` class can load any chess position from a FEN string. Handles piece placement, active color, castling rights, en passant square, and move clocks.

- **Make / Unmake Architecture** — Moves are done and undone using a state history stack. This allows the search to explore millions of positions without copying the board. Castling, en passant, promotions, and captures are all handled correctly in both directions.

### Move Generation
- **Pseudo-Legal + Legality Filter** — Moves are generated without checking for pins/discovered checks first (pseudo-legal), then filtered by actually making the move and checking if the king is in check. Not the fastest approach but it's correct and simple.

- **Complete Rule Support** — Castling (both sides, both colors), en passant, pawn double-push, and pawn promotion to all four piece types.

### UCI Protocol
- **Fully UCI Compatible** — Implements the `uci`, `isready`, `position`, `go`, `ucinewgame`, and `quit` commands. Works with CuteChess, Arena, and other UCI GUIs out of the box.

- **Terminal Play Mode** — If you don't have a GUI, just run the binary and type `play`. You can move pieces using algebraic notation (e.g., `e2e4`) and the engine will respond.

### Correctness
- **Perft Testing** — Built-in perft (performance test) and perft divide functions to verify move generation against known node counts. This is how I caught most of my bugs.


## Project Structure

```
ChessEngine/
├── include/
│   ├── Board.hpp              # Board state, FEN parsing, make/unmake
│   ├── MoveGenerator.hpp      # Move generation + search (alpha-beta, ID)
│   ├── TranspositionTable.hpp # TT with Zobrist probing + recording
│   ├── Zobrist.hpp            # Zobrist key initialization + hashing
│   ├── Move.hpp               # Move struct (from, to, flags)
│   └── Definitions.hpp        # Enums, constants, piece-square tables
│
├── src/
│   ├── main.cpp               # Entry point, UCI loop, terminal mode
│   ├── Board.cpp              # Board implementation
│   ├── MoveGenerator.cpp      # Move gen + search
│   └── Perft.cpp              # Perft testing
└── CMakeLists.txt
```

The codebase separates concerns cleanly — `Board` handles all state and rules, `MoveGenerator` handles move generation and search, `TranspositionTable` handles caching, and `Zobrist` handles hashing.


## How to Build

You'll need a C++ compiler with C++17 support and CMake.

```bash
git clone https://github.com/GodSonIsBack/ChessEngine.git
cd ChessEngine
cmake -S . -B build
cmake --build build
```

> **Note:** The build uses `-O3` optimization. Without it, the engine will be noticeably slower because the STL containers don't get their bounds-checking stripped out.

---

## How to Use

**Play in the terminal:**
```
./build/engine
> play
```
You play as white, type moves like `e2e4`, `g1f3`, etc. The engine plays as black.

**Use with a Chess GUI:**
Load the compiled binary into any UCI-compatible GUI (CuteChess, Arena, etc.). When the GUI sends `uci`, the engine automatically switches to protocol mode.

## Technical Decisions Worth Mentioning

A few choices I made and why:

- **Negamax over separate min/max** — With negamax, the same function handles both sides by just negating the returned score. Reducing the code size, and providing same result.

- **Incremental eval + hash updates** — Both the evaluation and the Zobrist hash are incrementally updated inside `makeMove()` / `unmakeMove()` rather than being recalculated. This keeps the per-node cost constant regardless of how many pieces are on the board.

- **State history stack instead of board copying** — The `StateInfo history[2048]` array acts as an undo stack. Each `makeMove` pushes the current castling rights, en passant square, evaluation, and hash. Each `unmakeMove` pops them. Making this an efficient way to do and undo the board state during Search.

- **TT move bonus of 20,000** — When the transposition table has a best move from a shallower search, that move is scored 20,000 points higher than any MVV-LVA capture. This ensures the iterative deepening best-move is always searched first, which is the main reason iterative deepening actually works.


## TO-DO

- [ ] Killer Move Heuristic — Store quiet moves (non-captures) that caused a beta cutoff at each depth. When ordering moves prioritise those killer moves.
- [ ] History Heuristic — Maintain a `[piece][toSquare]` table that accumulates a bonus every time a quiet move improves alpha
- [ ] Time Management — Dynamic depth based on remaining clock time instead of a fixed depth.
- [ ] Bitboard Representation — Replace the int[64] board with 64-bit bitboards for faster move generation.

## Learning Resources

📖 **Documentation**
- [Chess Programming Wiki](https://www.chessprogramming.org) — The go-to reference for everything chess engine related
- [Rustic Chess Engine Book](https://rustic-chess.org/) by Marcel Vanthoor — Step-by-step guide to building a chess engine from scratch

🎥 **Videos**
- [The Fascinating Programming of a Chess Engine](https://www.youtube.com/watch?v=w4FFX_otR-4) — Great high-level overview of how chess engines work *(Bartek Spitza)*
- [Algorithms Explained – Minimax and Alpha-Beta Pruning](https://www.youtube.com/watch?v=l-hh51ncgDI) — Clear visual explanation of the core search algorithm *(Sebastian Lague)*
- [Transposition Tables & Zobrist Keys](https://www.youtube.com/watch?v=QYNRvMolN20) — Walkthrough of TT implementation with Zobrist hashing *(Logic Crazy Chess)*
- [I Tried to Code a Chess Engine. It Broke Me.](https://www.youtube.com/watch?v=UqCXwP1F-ho) — The video that inspired this project *(commonLuke)*

## Author

**Patanjali Dwivedi**
*(B.Tech Pre-final at IIT-ISM Dhanbad)*

Built as a deep dive into game tree search, hashing algorithms, and low-level C++ performance optimization.