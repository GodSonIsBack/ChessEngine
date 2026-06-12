#pragma once

#include <vector>

#include "Move.hpp"
#include "Board.hpp"

class MoveGenerator
{
    private:
        //-------HELPER FUNCTIONS----------
        void genKnightMoves(Board &board, std::vector<Move> &moves);
        void genKingMoves(Board &board, std::vector<Move> &moves);  
        void genRookMoves(Board &board, std::vector<Move> &moves);
        void genBishopMoves(Board &board, std::vector<Move> &moves);
        void genQueenMoves(Board &board, std::vector<Move> &moves);
        void genPawnMoves(Board &board, std::vector<Move> &moves);
        
    public: 
        std::vector<Move> generateAllMoves(Board &board);
};