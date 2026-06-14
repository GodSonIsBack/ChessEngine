#pragma once

#include <vector>

#include "Move.hpp"
#include "Board.hpp"

class MoveGenerator
{
    private:
        //-------HELPER FUNCTIONS----------
        void genKingMoves(Board &board, std::vector<Move> &moves);  
        void genBishopMoves(Board &board, std::vector<Move> &moves);
        void genQueenMoves(Board &board, std::vector<Move> &moves);
        void genPawnMoves(Board &board, std::vector<Move> &moves);
        void genRookMoves(Board &board, std::vector<Move> &moves);
        void genKnightMoves(Board &board, std::vector<Move> &moves);
        
    public: 
        std::vector<Move> generateAllMoves(Board &board);
        std::vector<Move> generateLegalMoves(Board &board);

        //Performance Testing:
        long long perft(int depth, Board &board);
        void perftDivide(int depth, Board &board);

        // Search:
        int minimax(Board &board, int depth, int alpha, int beta);
        Move findBestMove(Board &board, int depth, std::vector<Move>& legalMoves);
};