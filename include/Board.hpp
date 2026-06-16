#pragma once
#include <vector>
#include "Definitions.hpp"
#include "Move.hpp"
#include <string>

struct StateInfo
{
    int castLingRight;
    Square enPassantSquare;
    int currentEval;
    unsigned long long currentHash;
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

        //en passant square:
            Square enPassantSquare;

        // Snapshot current state:
            StateInfo history[2048]; // Stack
            int histPtr; // Pointer to top of stack

        // King Cordinates:
            Square King_W;
            Square King_B;
        
        // Evaluation and Search:
            int currentEval;
            unsigned long long currentHash;

        // helper functions:
            Piece charToPiece(char c);
            char pieceToChar(Piece p);
            int getValueAndPST(Piece pieceType, Square sq);

    public:

        Board();
        void setPiece(Square s, Piece p);
        void printBoard();
        void loadFEN(std::string fen);

        void makeMove(Move move);
        void unmakeMove(Move move);

        bool isSquareAttacked(Square sq,Color attackingSide);

        int evaluate();
        
        // getters:
            Piece getPiece(Square s);  
            Color getSideToMove();
            Square getEnPassantSquare();
            Square getKingSq(Color playingSide);
            int getCastLingRight();
            int getCurrentEval();
            unsigned long long getCurrentHash();
};