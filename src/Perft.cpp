#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>

#include "../include/MoveGenerator.hpp"

// --- Helper Function for Formatting Numbers ---
std::string formatWithCommas(long long value) {
    std::string numWithCommas = std::to_string(value);
    int insertPosition = numWithCommas.length() - 3;
    while (insertPosition > 0) {
        numWithCommas.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return numWithCommas;
}

long long MoveGenerator::perft(int depth, Board &board)
{
    if (depth == 0) return 1ULL;

    long long nodes = 0;

    // Generate all pseudo-legal moves
    std::vector<Move> moveList = generateLegalMoves(board);

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
    std::vector<Move> moveList = generateLegalMoves(board);

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

void MoveGenerator::perftBenchmark(int maxDepth, Board &board)
{
    std::cout << "\n### Performance Benchmarks (Perft)\n";
    std::cout << "Tested on the initial chess starting position:\n\n";

    // Perfectly aligned Markdown Headers
    std::cout << "| " << std::left << std::setw(5)  << "Depth" 
              << " | " << std::left << std::setw(15) << "Nodes Visited" 
              << " | " << std::left << std::setw(10) << "Time Taken" 
              << " | " << std::left << std::setw(22) << "Nodes Per Second (NPS)" 
              << " |\n";
              
    std::cout << "|-------|-----------------|------------|------------------------|\n";

    for (int d = 1; d <= maxDepth; ++d) {
        auto start = std::chrono::high_resolution_clock::now();
        
        long long nodes = perft(d, board);
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        double timeTaken = elapsed.count();
        
        // Format Time
        std::stringstream timeStr;
        timeStr << std::fixed << std::setprecision(3) << timeTaken << "s";
        
        // Format NPS
        std::stringstream npsStr;
        if (timeTaken > 0.001) {
            double nps = nodes / timeTaken;
            if (nps > 1000000) {
                npsStr << std::fixed << std::setprecision(2) << (nps / 1000000.0) << "M";
            } else if (nps > 1000) {
                npsStr << std::fixed << std::setprecision(2) << (nps / 1000.0) << "k";
            } else {
                npsStr << static_cast<long long>(nps);
            }
        } else {
            npsStr << "-"; // Too fast to measure accurately
        }

        // Print aligned row
        std::cout << "| " << std::left  << std::setw(5)  << d 
                  << " | " << std::right << std::setw(15) << formatWithCommas(nodes) 
                  << " | " << std::right << std::setw(10) << timeStr.str() 
                  << " | " << std::right << std::setw(22) << npsStr.str() 
                  << " |\n";
    }
    std::cout << std::endl;
}