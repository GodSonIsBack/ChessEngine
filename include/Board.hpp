#pragma once
#include <vector>
#include "Definitions.hpp"
#include <string>

class Board
{
    private:
        //board:
        Piece board[64]={EMPTY};

        //side to move:
        Color sideToMove;

        //castling rights:
        int castLingRight; //a 4 bit integer
        //bits represent:
        // bit0: white kingside,
        // bit1: white queenside, 
        // bit2: black kingside, 
        // bit3: black queenside

        //en passant square:
        Square enPassantSquare;
        
        // helper functions:
        Piece charToPiece(char c);
        char pieceToChar(Piece p);

    public:
        Board();
        void setPiece(Square s, Piece p);
        void printBoard();
        void loadFEN(std::string fen);

};