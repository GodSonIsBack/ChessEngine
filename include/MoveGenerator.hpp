#pragma once

#include <vector>

#include "Move.hpp"
#include "Board.hpp"

class MoveGenerator
{
    private:
        //------CONSTANTS AND TABLES--------
            static const int MAX_PLY = 125;
            static const int TT_OFFSET = 300000;
            static const int MVV_LVA_OFFSET = 200000;
            static const int KILLER_OFFSET = 1000;
            static const int MAX_HISTORY = 10000;

            //Killer Moves and History Heuristic:
            Move KillerMoves[2][MAX_PLY];
            int historyTable[2][64][64] = {{{0}}};

        //-------MOVE GENERATORS----------
            void genKingMoves(Board &board, std::vector<Move> &moves);  
            void genBishopMoves(Board &board, std::vector<Move> &moves);
            void genQueenMoves(Board &board, std::vector<Move> &moves);
            void genPawnMoves(Board &board, std::vector<Move> &moves);
            void genRookMoves(Board &board, std::vector<Move> &moves);
            void genKnightMoves(Board &board, std::vector<Move> &moves);

        //-------MULTIPURPOSE HELPERS----------
            int getPieceValue(Piece PieceType);
            int scoreMove(Move move,Board &board,int ply);

            // TT Score Translators (for Mates)
            int valueToTT(int score, int ply);
            int valueFromTT(int score, int ply);

            //Killer Moves and History Heuristics:
            void storeKillerMove(Move move,int ply);
            void updateHistory(Move move, int depth, bool good, Color sideToMove);
        
    public: 
        std::vector<Move> generateAllMoves(Board &board);
        std::vector<Move> generateLegalMoves(Board &board);

        //Performance Testing:
        long long perft(int depth, Board &board);
        void perftDivide(int depth, Board &board);

        // Search:
        int minimax(Board &board, int depth, int ply, int alpha, int beta);
        Move searchRoot(Board &board, int depth, int ply, std::vector<Move> &legalMoves, Move prevBest);
        Move findBestMove(Board &board, int targetDepth, std::vector<Move>& legalMoves);

        //History:
        void clearHistory();
};