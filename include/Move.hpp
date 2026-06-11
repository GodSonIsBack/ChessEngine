#pragma once
#include "Definitions.hpp"

struct Move
{
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
};