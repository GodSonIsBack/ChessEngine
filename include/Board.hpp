#pragma once
#include <vector>
#include "Definitions.hpp"
#include "Move.hpp"
#include <string>

struct StateInfo
{
    int castLingRight;
    Square enPassantSquare;
}

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

        // helper functions:
            Piece charToPiece(char c);
            char pieceToChar(Piece p);

    public:
    
        Board();
        void setPiece(Square s, Piece p);
        void printBoard();
        void loadFEN(std::string fen);

        void makeMove(Move move);
        void unmakeMove(Move move);

        // getters:
            Piece getPiece(Square s);  
            Color getSideToMove();
            Square getEnPassantSquare();
            int getCastLingRight();
};