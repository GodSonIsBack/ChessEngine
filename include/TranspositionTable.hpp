#pragma once

#include <vector>

#include "Definitions.hpp"
#include "Move.hpp"

enum TTFlag
{
    TT_EXACT,
    TT_LOWERBOUND,
    TT_UPPERBOUND
};

struct TTEntry
{
    unsigned long long zobristHash;
    int score;
    int depth;
    Move bestMove;
    TTFlag flag;

    TTEntry():
        zobristHash(0),
        score(0),
        depth(0),
        flag(TT_EXACT),
        bestMove(Move())
        {}
};

const int TT_SIZE = 1048576; // 1MB (2^20)

class TranspositionTable
{
    private:
        std::vector<TTEntry> ttable;

    public:
        TranspositionTable()
        {
            ttable.resize(TT_SIZE);   
        }

        void clear()
        {

        }

        void recordHash(unsigned long long hash, int score, 
            int depth, Move bestMove, TTFlag flag)
        {
            int index = hash & (TT_SIZE - 1);

            //Store in the calculated index:
            ttable[index].zobristHash = hash;
            ttable[index].score = score;
            ttable[index].depth = depth;
            ttable[index].bestMove = bestMove;
            ttable[index].flag = flag;
        }

        bool probeTable(unsigned long long hash,int &returnScore,
            int depth, Move &ttMove, int alpha, int beta)
        {
            //Finding the Slot
            int index = hash & (TT_SIZE - 1);
            TTEntry &entry = ttable[index];
            
            if(entry.zobristHash != hash) return false; //Collision

            //We grab the bestMove regardless of depth
            ttMove = entry.bestMove;

            if(entry.depth >= depth)
            {
                if(entry.flag == TT_EXACT){
                    returnScore = entry.score;
                    return true;
                }

                //To Trigger new Beta Cutoff
                if(entry.flag == TT_LOWERBOUND && entry.score >= beta)
                {
                    returnScore = entry.score;
                    return true;
                }

                //Trigget new Alpha Fail:
                if(entry.flag == TT_UPPERBOUND && entry.score <= alpha)
                {
                    returnScore = entry.score;
                    return true;
                }
            }

            return false;
        }
};

//Global Instance:
inline TranspositionTable TT;