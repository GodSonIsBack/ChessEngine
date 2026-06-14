#include "../include/MoveGenerator.hpp"
#include <vector>
#include <iostream>

std::string SqtoString(Square sq)
{
    std::string s="";
    s+=(char)(sq%8+'a');
    s+=(char)(sq/8+'1');
    return s;
}

Piece charToPiece(char c)
    {
        switch (c)
        {
            case 'P': return W_PAWN;
            case 'N': return W_KNIGHT;
            case 'B': return W_BISHOP;
            case 'R': return W_ROOK;
            case 'Q': return W_QUEEN;
            case 'K': return W_KING;
            case 'p': return B_PAWN;
            case 'n': return B_KNIGHT;
            case 'b': return B_BISHOP;
            case 'r': return B_ROOK;
            case 'q': return B_QUEEN;
            case 'k': return B_KING;
            default:  return EMPTY;
        }
    }

Move parseUserMove(std::string moveStr, std::vector<Move> &legalMoves)
{
    if(moveStr.length() < 4) return Move();

    int fromFile = moveStr[0] - 'a';
    int fromRank = moveStr[1] - '1';
    int toFile = moveStr[2] - 'a';
    int toRank = moveStr[3] - '1';

    Piece promotionPiece = (moveStr.length() == 5) ? 
            charToPiece(moveStr[4]) : EMPTY;

    Square from = (Square)(fromRank * 8 + fromFile);
    Square to = (Square)(toRank * 8 + toFile);

    for(auto move : legalMoves)
    {
        if(move.from == from && move.to == to && move.promotionPiece == promotionPiece)
        {
            return move;
        }
    }

    return Move();
}

int main()
{
    Board board;
    MoveGenerator generator;

    std::cout << "====================================\n";
    std::cout << "      C++ CHESS ENGINE ONLINE       \n";
    std::cout << "====================================\n";

    while (true) {
        board.printBoard();
        std::vector<Move> legalMoves = generator.generateLegalMoves(board);

        if (legalMoves.empty()) {
            Color playingSide = board.getSideToMove();
            Color attackingSide = (Color)(playingSide ^ 1);

            if(board.isSquareAttacked(board.getKingSq(playingSide), attackingSide))
                std::cout << "CHECKMATE! "<< (playingSide == WHITE ? "BLACK" : "WHITE") << "wins \n";
            else 
                std::cout << "STALEMATE!!";
            
            break;
        }

        std::cout << "Enter your move (e.g., e2e4): ";

        //User plays WHITE:
        if(board.getSideToMove() == WHITE)
        {
            std::string userMoveStr;
            std::cin >> userMoveStr;

            Move userMove = parseUserMove(userMoveStr, legalMoves);

            if(userMove.from == 64)
            {
                std::cout<<"Invalid or illegal move. Try again" <<std::endl;
                continue;
            }

            board.makeMove(userMove);
        }
        else
        {
            std::cout << "Engine is thinking...\n";

            Move aiMove = generator.findBestMove(board, 6, legalMoves);

            std::cout << "Engine plays: " 
                      << SqtoString(aiMove.from) 
                      << SqtoString(aiMove.to) << "\n";

            board.makeMove(aiMove);
        }
    }
    std::cout << "Game Over. Thanks for playing!\n";
    return 0;
}