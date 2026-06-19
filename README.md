# Chess Engine in C++

A chess engine built completely from scratch in C++17 without the use of any libraries or frameworks.

The engine provides three modes:
- **UCI Mode** — Works with any UCI-compatible chess GUI (CuteChess, Arena, etc.)
- **Terminal Mode** — Play against the engine directly in the console
- **Perft Mode** — Run performance tests to verify move generation correctness

Capable of achieving an average time of *2.5s* at depth 7.
Searches comfortably at depth 8 with multiple layers of search optimizations stacked on top of each other.


## Features

### Search Algorithm
- **Negamax with Alpha-Beta Pruning** — Implemented NegaMax instead of the plain minimax with alpha-beta bounds to prune off branches not affecting the results, massively reducing the number of positions the engine has to look at.

- **Iterative Deepening** — Rather than jumping straight to the target depth, the engine searches incrementally from depth 1 up to the target. Each iteration seeds the transposition table and produces a best move, making deeper searches complete faster.

- **Mate Distance Pruning** — Checkmate scores are adjusted by ply distance (`-MATE_VALUE + ply`), so the engine always prefers the fastest mate over a slower one.

### Move Ordering
- **Transposition Table Move Priority** — assigns a massive score bonus (`TT_OFFSET = 300,000`) to the move returned from Transposition Table having it to be searched first, leading to a massive speedup in the engine.
- **MVV-LVA (Most Valuable Victim – Least Valuable Attacker)** — Sorts captures according to the **MVV-LVA** move ordering technique making the alpha-beta prune more efficient and searching the tree faster.



- **Killer Move Heuristic** — Stores up to 2 killer moves per ply. These moves are prioritized in sibling nodes since a move that refuted one position is likely strong in similar positions at the same depth.

- **History Heuristic** — Maintains a `[side][from][to]` table that tracks which quiet moves have historically been good across the entire search. Rewards and Penalizes the moves using **History Gravity Formula** ensuring more efficient move ordering.


### Transposition Table
- **Incremental Zobrist Hashing** — Every board position gets a unique 64-bit hash using XOR of pre-generated random numbers. The hash updates incrementally on each move (no need to rehash the entire board), making it essentially free.

- **1M Entry Hash Table** — A 2^20 entry table indexed by bitwise masking. Stores the score, depth, best move, and a flag (EXACT / LOWERBOUND / UPPERBOUND) for each position. Prevents the engine from re-evaluating positions it has already seen.

### Evaluation
- **Material Counting + Piece-Square Tables(PSTs)** — The evaluation is a sum of material values (pawn=100, knight/bishop=300, rook=500, queen=900) and positional bonuses from pre-written **PSTs** encouraging piece mobility.

- **Incremental Evaluation** — Have the `makeMove` / `unmakeMove` functions incrementally update the score at each move optimising the evaluation from `O(N)` to `O(1)`. 

### Board Representation
- **Array-Based (int[64])** — A simple flat array where index 0 = A1 and index 63 = H8. Pieces are stored as enum values.

- **Full FEN Parsing** — `loadFEN` function in `Board` class can load any chess position from a FEN string. Handles piece placement, active color, castling rights, en passant square, and move clocks.

- **Make / Unmake Architecture** — Moves are done and undone using a state history stack. This allows the search to explore millions of positions without copying the board. Castling, en passant, promotions, and captures are all handled correctly in both directions.

### Move Generation
- **Pseudo-Legal + Legality Filter** — Moves are generated without checking for pins/discovered checks first (pseudo-legal), then filtered by actually making the move and checking if the king is in check.

- **Complete Rule Support** — Castling (both sides, both colors), en passant, pawn double-push, and pawn promotion to all four piece types.

### UCI Protocol
- **Fully UCI Compatible** — Implements `uci`, `isready`, `position`, `go`, `go perft`, `ucinewgame`, and `quit`. Works with CuteChess, Arena, and other UCI GUIs out of the box.

- **Terminal Play Mode** — Run the binary and type `play`. Move pieces using algebraic notation (e.g., `e2e4`) and the engine responds.

- **Perft Mode** — Run the binary and type `perft` to test move generation against any FEN position at any depth. Also available via UCI with `go perft <depth>`.

### Correctness
- **Perft Testing** — Built-in perft and perft divide functions to verify move generation against known node counts.


## Project Structure

```
ChessEngine/
├── include/
│   ├── Board.hpp              # Board state, FEN parsing, make/unmake
│   ├── MoveGenerator.hpp      # Move generation, search, Killer/History tables
│   ├── TranspositionTable.hpp # TT with Zobrist probing + recording
│   ├── Zobrist.hpp            # Zobrist key initialization + hashing
│   ├── Move.hpp               # Move struct (from, to, flags)
│   └── Definitions.hpp        # Enums, constants, piece-square tables
│
├── src/
│   ├── main.cpp               # Entry point, UCI loop, terminal + perft modes
│   ├── Board.cpp              # Board implementation
│   ├── MoveGenerator.cpp      # Move gen + search + heuristics
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

> **Note:** The build uses `-O3` optimization. Without it, the engine will be noticeably slower due to unoptimized STL checks.


## How to Use

**Play in the terminal:**
```
./build/engine
> play
```
You play as white, type moves like `e2e4`, `g1f3`, etc. The engine plays as black.

**Use with a Chess GUI:**
Load the compiled binary into any UCI-compatible GUI (CuteChess, Arena, etc.). The GUI sends `uci` and the engine automatically switches to protocol mode.

**Run perft tests:**
```
./build/engine
> perft
```
Enter a FEN string (or press enter for the starting position) and a depth to verify move generation.

## Technical Decisions Worth Mentioning

A few choices I made and why:

- **Negamax over separate min/max** — With negamax, the same function handles both sides by just negating the returned score. Reducing the code size, and providing same result.

- **Incremental eval + hash updates** — Both the evaluation and the Zobrist hash are incrementally updated inside `makeMove()` / `unmakeMove()` rather than being recalculated. This keeps the per-node cost constant regardless of how many pieces are on the board.

- **State history stack instead of board copying** — The `StateInfo history[2048]` array acts as an undo stack. Each `makeMove` pushes the current castling rights, en passant square, evaluation, and hash. Each `unmakeMove` pops them — far cheaper than cloning the board.

- **Layered move ordering with explicit score tiers** — Moves are scored using named constants (`TT_OFFSET = 300,000` > `MVV_LVA_OFFSET = 200,000` > `KILLER_OFFSET = 1,000` > history scores). This guarantees TT moves are always searched before captures, captures before killers, and killers before quiet history moves.

- **History gravity** — The history table uses the gravity formula (`bonus - current * |bonus| / MAX`) from the Chess Programming Wiki to prevent scores from growing without bound. Good moves get rewarded, moves that wasted search time get penalized.


## TO-DO

- [ ] Quiescence Search — Continue searching captures at depth 0 to avoid the horizon effect
- [ ] Time Management — Dynamic depth based on remaining clock time instead of a fixed depth
- [ ] Bitboard Representation — Replace the int[64] board with 64-bit bitboards for faster move generation

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