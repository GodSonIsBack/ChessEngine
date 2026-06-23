#pragma once
#include "Definitions.hpp"
#include "Move.hpp"
#include <string>

struct StateInfo
{
    int castLingRight;
    Square enPassantSquare;
    Score currentEval;
    int gamePhase;
    unsigned long long currentHash;
    int halfMoveClock;
    int fullMoveNumber;
};

class Board
{
    private:
        //board:
            Piece board[64]={EMPTY};

        //side to move:
            Color sideToMove;

        //castling rights:
            int castLingRight; // BQ,BK,WQ,WK

            int halfMoveClock;
            int fullMoveNumber;

        //en passant square:
            Square enPassantSquare;

        // Snapshot current state:
            StateInfo history[2048]; // Stack
            int histPtr; // Pointer to top of stack

        // King Cordinates:
            Square King_W;
            Square King_B;
        
        // Evaluation and Search:
            Score currentEval; // contaings both mid and end game eval
            int gamePhase; // Tracks remaining material phase (used in interpolation)
            unsigned long long currentHash;
        
        //Tapered Evaluation:
            static Score PST[13][64]; // Master Table
            static bool tablesInitialized;
            void initTables();

        // helper functions:
            Piece charToPiece(char c);
            char pieceToChar(Piece p);
            
            Score getPieceScore(Piece pieceType, Square sq);
            int getPiecePhase(Piece pieceType);

    public:

        Board();
        void setPiece(Square s, Piece p);
        void printBoard();
        void loadFEN(std::string fen);

        void makeMove(Move move);
        void unmakeMove(Move move);

        bool isSquareAttacked(Square sq,Color attackingSide);

        int evaluate();

        bool isRepetition();
        
        // getters:
            Piece getPiece(Square s);  
            Color getSideToMove();
            Square getEnPassantSquare();
            Square getKingSq(Color playingSide);
            int getCastLingRight();
            int getCurrentEval();
            int getHalfMoveClock();
            unsigned long long getCurrentHash();
};