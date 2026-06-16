#pragma once

#include <random>

#include "Definitions.hpp"
#include "Board.hpp"

namespace Zobrist
{
    // Zobrist Randoms:
        inline unsigned long long pieceKeys[12][64]; // 12 pieces, 64 squares
        inline unsigned long long enPassantKeys[8]; // 8 for all files
        inline unsigned long long castleKeys[16]; // Similar to castlingRights
        inline unsigned long long sideKey; // Single Random for Black's turn

    inline void initZobrist()
        {
            //64-bit random number generator:
            std::mt19937_64 rng(1070372ULL); 
            std::uniform_int_distribution<unsigned long long> dist(0, 0xFFFFFFFFFFFFFFFF);

            for(int piece = 0; piece < 12; piece++)
                for(int sq = 0; sq < 64; sq++)
                    pieceKeys[piece][sq] = dist(rng);

            for(int file = 0; file < 8; file++)
                enPassantKeys[file] = dist(rng);

            for(int i = 0; i < 16; i++)
                castleKeys[i] = dist(rng);

            sideKey = dist(rng);
        }

    inline unsigned long long generateHash(Board &board)
        {
            unsigned long long finalHash=0;

            for(int sq = 0; sq < 64; sq++)
            {
                Piece currPiece = board.getPiece((Square) sq);
                if(currPiece != EMPTY) finalHash ^= pieceKeys[currPiece - 1][sq];
            }

            if(board.getEnPassantSquare() != NO_SQ)
                // Xor the corresponding file
                finalHash ^= enPassantKeys[board.getEnPassantSquare() % 8];

            finalHash ^= castleKeys[board.getCastLingRight()];
            
            if(board.getSideToMove() == BLACK) finalHash ^= sideKey;

            return finalHash;
        }
}