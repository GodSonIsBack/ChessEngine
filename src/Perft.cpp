#include <vector>
#include <iostream>

#include "../include/MoveGenerator.hpp"

long long MoveGenerator::perft(int depth, Board &board)
{
    if (depth == 0) return 1ULL;

    long long nodes = 0;

    // Generate all pseudo-legal moves
    std::vector<Move> moveList = generateAllMoves(board);

    for (const auto& move : moveList)
    {
        board.makeMove(move);
        nodes += perft(depth - 1, board);
        board.unmakeMove(move);
    }

    return nodes;
}

void MoveGenerator::perftDivide(int depth, Board &board)
{
    if (depth == 0) return;

    long long totalNodes = 0;
    std::vector<Move> moveList = generateAllMoves(board);

    std::cout << std::endl<<"--- PERFT DIVIDE-Depth: " << depth << "---" << std::endl;

    for (const auto& move : moveList)
    {
        //perform move
            board.makeMove(move);
        // Count nodes
            long long nodesForMove = perft(depth - 1, board);
        // Add to total
            totalNodes += nodesForMove;
        //Undo move
            board.unmakeMove(move);

        // Print the move and its branch size
        std::cout << "Move from " << move.from << " to " << move.to << " : " << nodesForMove << std::endl;
    }

    std::cout << std::endl<<"Total Positions Found: " << totalNodes << std::endl;
}