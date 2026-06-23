#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../include/Board.hpp"
#include "../include/MoveGenerator.hpp"
#include "../include/Zobrist.hpp"
#include "../include/TranspositionTable.hpp" // To clear TT on new game

// --- Helper Functions ---
std::string SqtoString(Square sq) {
    std::string s = "";
    s += (char)(sq % 8 + 'a');
    s += (char)(sq / 8 + '1');
    return s;
}

Piece charToPiece(char c) {
    switch (c) {
        case 'P': return W_PAWN; case 'N': return W_KNIGHT; case 'B': return W_BISHOP;
        case 'R': return W_ROOK; case 'Q': return W_QUEEN; case 'K': return W_KING;
        case 'p': return B_PAWN; case 'n': return B_KNIGHT; case 'b': return B_BISHOP;
        case 'r': return B_ROOK; case 'q': return B_QUEEN; case 'k': return B_KING;
        default:  return EMPTY;
    }
}

Move parseUserMove(std::string moveStr, const std::vector<Move>& legalMoves, Color sideToMove) {
    if(moveStr.length() < 4) return Move();
    int fromFile = moveStr[0] - 'a';
    int fromRank = moveStr[1] - '1';
    int toFile = moveStr[2] - 'a';
    int toRank = moveStr[3] - '1';

    Piece promotionPiece = EMPTY;
    if(moveStr.length() == 5) {
        char p = moveStr[4];
        if (sideToMove == WHITE) {
            if (p == 'q') promotionPiece = W_QUEEN;
            else if (p == 'r') promotionPiece = W_ROOK;
            else if (p == 'b') promotionPiece = W_BISHOP;
            else if (p == 'n') promotionPiece = W_KNIGHT;
        } else {
            if (p == 'q') promotionPiece = B_QUEEN;
            else if (p == 'r') promotionPiece = B_ROOK;
            else if (p == 'b') promotionPiece = B_BISHOP;
            else if (p == 'n') promotionPiece = B_KNIGHT;
        }
    }
    
    Square from = (Square)(fromRank * 8 + fromFile);
    Square to = (Square)(toRank * 8 + toFile);

    for(auto move : legalMoves) {
        if(move.from == from && move.to == to && move.promotionPiece == promotionPiece) {
            return move;
        }
    }
    return Move(); // Returns an empty/invalid move if not found
}

// ---The Terminal Interactive Mode ---
void runTerminal() {
    Board board;
    MoveGenerator generator;

    std::cout << "====================================\n";
    std::cout << "      C++ CHESS ENGINE ONLINE       \n";
    std::cout << "====================================\n";

    while (true) {
        board.printBoard();
        std::vector<Move> legalMoves = generator.generateLegalMoves(board);

        if (legalMoves.empty()) {
            Color playingSide = board.getSideToMove();
            Color attackingSide = (Color)(playingSide ^ 1);

            if(board.isSquareAttacked(board.getKingSq(playingSide), attackingSide))
                std::cout << "CHECKMATE! " << (playingSide == WHITE ? "BLACK" : "WHITE") << " wins!\n";
            else 
                std::cout << "STALEMATE!!\n";
            
            break;
        }

        std::cout << "Enter your move (e.g., e2e4): ";

        if(board.getSideToMove() == WHITE) {
            std::string userMoveStr;
            std::cin >> userMoveStr;

            Move userMove = parseUserMove(userMoveStr, legalMoves, WHITE);

            if(userMove.from == NO_SQ) {
                std::cout << "Invalid or illegal move. Try again.\n";
                continue;
            }

            board.makeMove(userMove);
        } else {
            std::cout << "Engine is thinking...\n";

            // give the terminal mode "infinite" time to reach Depth 7
            generator.enableTimer(2000000000);

            Move aiMove = generator.findBestMove(board, 8, legalMoves);

            std::cout << "Engine plays: " 
                      << SqtoString(aiMove.from) 
                      << SqtoString(aiMove.to) << "\n";

            board.makeMove(aiMove);
        }
    }
    std::cout << "Game Over. Thanks for playing!\n";
}

// --- Universal Chess Interface (UCI) Mode ---
void runUCI() {
    Board board;
    MoveGenerator generator;

    std::cout << "id name MyCppEngine" << std::endl;
    std::cout << "id author Patanjali" << std::endl;
    std::cout << "uciok" << std::endl;

    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string command;
        ss >> command; 

        if (command == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "ucinewgame") {
            board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            TT.clear(); // Wiping the TTable for a new game
            generator.clearHistory(); // Wipes the History Table Clean
        } 
        else if (command == "position") {
            std::string token;
            ss >> token;

            if (token == "startpos") {
                board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                ss >> token; // if there is a "moves" keyword next
            } 
            else if (token == "fen") {
                std::string fen = "";
                for (int i = 0; i < 6; i++) {
                    ss >> token;
                    fen += token + " ";
                }
                board.loadFEN(fen);
                ss >> token; // Check for "moves" keyword
            }

            // Sync the board state with the GUI's move history
            if (token == "moves") {
                std::string moveStr;
                while (ss >> moveStr) {
                    std::vector<Move> legalMoves = generator.generateLegalMoves(board);
                    Move m = parseUserMove(moveStr, legalMoves, board.getSideToMove());
                    if (m.from != NO_SQ) {
                        board.makeMove(m);
                    }
                }
            }
        } 
        else if (command == "go") {
            int depth = -1;
            int wtime = 0;
            int btime = 0;
            int winc = 0;
            int binc = 0;
            int movetime = 0;

            std::string token;
            bool isPerft = false;
            
            // Look for GUI depth commands
            while (ss >> token) {
                if (token == "depth") ss >> depth;
                else if (token == "perft") { ss >> depth; isPerft = true; }
                else if (token == "wtime") ss >> wtime;
                else if (token == "btime") ss >> btime;
                else if (token == "winc") ss >> winc;
                else if (token == "binc") ss >> binc;
                else if (token == "movetime") ss >> movetime;
            }

            if (isPerft) {
                generator.perftDivide(depth, board);
                continue; // Skip search
            }

            int timeLimit = -1;
            if(movetime > 0) timeLimit = movetime - 50;
            else if(wtime > 0 || btime > 0)
            {
                int timeRemaining = (board.getSideToMove() == WHITE) ? wtime : btime;
                int increment = (board.getSideToMove() == WHITE) ? winc : binc;

                //Time Allocation formula:
                timeLimit = (timeRemaining / 40) + (increment / 2);

                //Never allocate more time that you have
                if(timeLimit >= timeRemaining)
                    timeLimit = timeRemaining - 50; // Leave 50ms for processing/netorking overhead

                // Absolute minimum time to avoid crashing or passing 0
                if (timeLimit < 1) {
                    timeLimit = 1; 
                }
            }

            if (timeLimit != -1) { 
                // if timeLimit exists
                depth = 125; // equal to MAX_PLY 
            } else { 
                // No time limit provided
                if (depth == -1) depth = 7; // Default testing depth
                timeLimit = 2000000000; // "infinite" time so the timer never aborts
            }

            // Start the stopwatch!
            generator.enableTimer(timeLimit);

            std::vector<Move> legalMoves = generator.generateLegalMoves(board);
            Move bestMove = generator.findBestMove(board, depth, legalMoves);
            
            if (bestMove.from == NO_SQ) {
                std::cout << "bestmove 0000" << std::endl; // Resign/Null move if mated
                continue;
            }
            
            std::string moveString = SqtoString(bestMove.from) + SqtoString(bestMove.to);
            
            if (bestMove.promotionPiece != EMPTY) {
                if (bestMove.promotionPiece == W_QUEEN || bestMove.promotionPiece == B_QUEEN) moveString += 'q';
                else if (bestMove.promotionPiece == W_ROOK || bestMove.promotionPiece == B_ROOK) moveString += 'r';
                else if (bestMove.promotionPiece == W_BISHOP || bestMove.promotionPiece == B_BISHOP) moveString += 'b';
                else if (bestMove.promotionPiece == W_KNIGHT || bestMove.promotionPiece == B_KNIGHT) moveString += 'n';
            }

            // Send the move back to the GUI
            std::cout << "bestmove " << moveString << std::endl;
        }
        else if (command == "quit") {
            break;
        }
    }
}

// --- Performance Testing (Perft) Mode ---
void runPerft() {
    Board board;
    MoveGenerator generator;
    std::string fen;
    int depth;

    std::cout << "====================================\n";
    std::cout << "         PERFT TESTING MODE         \n";
    std::cout << "====================================\n";
    std::cout << "Enter FEN (or type 'start' for default): ";
    
    std::getline(std::cin, fen);
    if (fen == "start" || fen.empty()) {
        board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else {
        board.loadFEN(fen);
    }

    std::cout << "Enter depth: ";
    std::cin >> depth;
    std::cin.ignore(); // clear the newline character

    std::cout << "\nType 'b' for Markdown Benchmark Table, or 'd' for Perft Divide: ";
    std::string mode;
    std::getline(std::cin, mode);

    std::cout << "\nRunning Perft at depth " << depth << "...\n";
    
    if (mode == "b") {
        generator.perftBenchmark(depth, board);
    } else {
        generator.perftDivide(depth, board); 
    }
    
    std::cout << "\nPerft complete. Exiting.\n";
}

int main() {
    // Generate the random 64-bit keys before the engine does anything else
    Zobrist::initZobrist();

    std::cout << "Type 'uci' for GUI Mode, or 'play' for Terminal Mode, or 'perft' for Testing: " << std::endl;
    
    std::string firstInput;
    if (std::getline(std::cin, firstInput)) {
        if (firstInput == "uci") {
            runUCI();
        } else if (firstInput == "perft") {
            runPerft();
        } else {
            runTerminal();
        }
    }

    return 0;
}