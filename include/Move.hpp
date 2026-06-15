#pragma once
#include "Definitions.hpp"

struct Move
{
    Move() : 
        from(NO_SQ), 
        to(NO_SQ), 
        capturedPiece(EMPTY), 
        promotionPiece(EMPTY), 
        isCastling(false), 
        isEnPassant(false),
        score(0)
    {}

    Move(Square sq, Square targetSq, Piece targetPiece, 
        Piece promo = EMPTY,bool castling = false, bool enPassant = false):
        from(sq),
        to(targetSq),
        capturedPiece(targetPiece),
        promotionPiece(promo),
        isCastling(castling),
        isEnPassant(enPassant),
        score(0)
    {}

    //base information
    Square from;
    Square to;

    // info related to captures
    Piece capturedPiece;

    // info related to promotion
    Piece promotionPiece;

    // special moves
    bool isCastling;
    bool isEnPassant;

    //Move Ordering:
    int score;
};