#include <vector>

#include "../include/MoveGenerator.hpp"
#include "../include/Board.hpp"
#include "../include/Move.hpp"

void MoveGenerator::genKnightMoves(Board &board, std::vector<Move> &moves)
{
    // relative offsets for knight moves
    int offsets[] = {15, 17, -17, -15, 6, 10, -6, -10};

    Color c = board.getSideToMove();
    Piece Knight = (c == WHITE) ? W_KNIGHT : B_KNIGHT;

    for(int sq = 0; sq < 64; sq++)
    {
        if(board.getPiece(sq)!=Knight) continue;

        int file = sq % 8;
        for(auto offset : offsets)
        {
            // IGNORE INVALID MOVES:
                // Wrap-around:
                if ((offset == -10 || offset == 6) && file < 2) continue;
                if ((offset == -17 || offset == 15) && file < 1) continue;
                if ((offset == -15 || offset == 17) && file > 6) continue;
                if ((offset == -6 || offset == 10) && file > 5) continue;

                // Out of Bounds:
                int targetSq = sq + offset;
                if(targetSq < 0 || targetSq > 63) continue;

                // Same color Piece:
                Piece targetPiece = board.getPiece(targetSq);
                if(targetPiece!=EMPTY)
                {
                    bool isWhitePiece = (targetPiece >= W_PAWN && targetPiece <= W_KING);
                    bool isFriendly = (c == WHITE) ? isWhitePiece : !isWhitePiece;
                    if(isFriendly) continue;
                }

            // Add valid move
            Move move;
            move.from = sq;
            move.to = targetSq;
            move.capturedPiece = targetPiece;
            move.promotionPiece = EMPTY;
            move.isCastling = false;
            move.isEnPassant = false;
            moves.push_back(move);
        }
    }
}

void MoveGenerator::genKingMoves(Board &board, std::vector<Move> &moves)
{
    // relative offsets for king moves
    int offsets[] = {1, -1, 8, -8, 9, -9, 7, -7};

    Color c = board.getSideToMove();
    Piece KING = (c == WHITE) ? W_KING : B_KING;

    for(int sq = 0; sq < 64; sq++)
    {
        if(board.getPiece(sq) != KING) continue;

        int file = sq % 8;
        for(auto offset : offsets)
        {
            // IGNORE INVALID MOVES:
                // Wrap-around:
                if (file == 0 && (offset == -1 || offset == -9 || offset == 7)) 
                    continue;
                if (file == 7 && (offset == 1 || offset == -7 || offset == 9)) 
                    continue;

                // Out of Bounds:
                int targetSq = sq + offset;
                if(targetSq < 0 || targetSq > 63) continue;

                // Same color Piece:
                Piece targetPiece = board.getPiece(targetSq);
                if(targetPiece != EMPTY)
                {
                    bool isWhitePiece = (targetPiece >= W_PAWN && targetPiece <= W_KING);
                    bool isFriendly = (c == WHITE) ? isWhitePiece : !isWhitePiece;
                    if(isFriendly) continue;
                }
            
            //Add valid move
            Move move;
            move.from = sq;
            move.to = targetSq;
            move.capturedPiece = targetPiece;
            move.promotionPiece = EMPTY;
            move.isCastling = false;
            move.isEnPassant = false;
            moves.push_back(move);
        }
    }
}

void MoveGenerator::genRookMoves(Board &board, std::vector<Move> &moves)
{
    int offsets[]= {+8,-8,+1,-1};

    Color c = board.getSideToMove();
    Piece ROOK = (c==WHITE)?W_ROOK:B_ROOK; 

    for(int sq = 0;sq < 64; sq++)
    {
        if(board.getPiece(sq) != ROOK) continue;

        for(auto offset : offsets)
        {
            int currSq = sq;
            while(true)
            {
                int file = currSq % 8;

                //Wrap-Around:
                if(file == 7 && offset == 1) break;
                if(file == 0 && offset == -1) break;

                //Out of Bound:
                int targetSq = currSq + offset;
                if(targetSq < 0 || targetSq > 63) break;

                Piece targetPiece = board.getPiece(targetSq);
                if(targetPiece != EMPTY)
                {
                    bool isWhitePiece = (targetPiece >= W_PAWN && targetPiece <= W_KING);
                    bool isFriendly = (c == WHITE) ? isWhitePiece : !isWhitePiece;
                    if(isFriendly) break;
                }
                
                //Add Valid Move:
                Move move;
                move.from = sq;
                move.to = targetSq;
                move.capturedPiece = targetPiece;
                move.promotionPiece = EMPTY;
                move.isCastling = false;
                move.isEnPassant = false;
                moves.push_back(move);

                // If it's a capture break
                if (targetPiece != EMPTY) break;

                currSq = targetSq;
            }
        }
    }
}

void MoveGenerator::genBishopMoves(Board &board, std::vector<Move> &moves)
{
    int offsets[]= {+9,-9,+7,-7};

    Color c = board.getSideToMove();
    Piece BISHOP = (c==WHITE)?W_BISHOP:B_BISHOP; 

    for(int sq = 0;sq < 64; sq++)
    {
        if(board.getPiece(sq) != BISHOP) continue;

        for(auto offset : offsets)
        {
            int currSq = sq;
            while(true)
            {
                int file = currSq % 8;

                //Wrap-Around:
                if(file == 7 && (offset == 9 || offest == -7)) break;
                if(file == 0 && (offset == 7 || offest == -9)) break;

                //Out of Bound:
                int targetSq = currSq + offset;
                if(targetSq < 0 || targetSq > 63) break;

                Piece targetPiece = board.getPiece(targetSq);
                if(targetPiece != EMPTY)
                {
                    bool isWhitePiece = (targetPiece >= W_PAWN && targetPiece <= W_KING);
                    bool isFriendly = (c == WHITE) ? isWhitePiece : !isWhitePiece;
                    if(isFriendly) break;
                }
                
                //Add Valid Move:
                Move move;
                move.from = sq;
                move.to = targetSq;
                move.capturedPiece = targetPiece;
                move.promotionPiece = EMPTY;
                move.isCastling = false;
                move.isEnPassant = false;
                moves.push_back(move);

                // If it's a capture break
                if (targetPiece != EMPTY) break;

                currSq = targetSq;
            }
        }
    }
}

void MoveGenerator::genQueenMoves(Board &board, std::vector<Move> &moves)
{
    int offsets[]= {+9,-9,+7,-7,+8,-8,+1,-1};

    Color c = board.getSideToMove();
    Piece QUEEN = (c==WHITE)?W_QUEEN:B_QUEEN; 

    for(int sq = 0;sq < 64; sq++)
    {
        if(board.getPiece(sq) != QUEEN) continue;

        for(auto offset : offsets)
        {
            int currSq = sq;
            while(true)
            {
                int file = currSq % 8;

                //Wrap-Around:
                if(file == 7 && (offset == 9 || offest == -7 || offset == 1)) break;
                if(file == 0 && (offset == 7 || offest == -9 || offset == -1)) break;

                //Out of Bound:
                int targetSq = currSq + offset;
                if(targetSq < 0 || targetSq > 63) break;

                Piece targetPiece = board.getPiece(targetSq);
                if(targetPiece != EMPTY)
                {
                    bool isWhitePiece = (targetPiece >= W_PAWN && targetPiece <= W_KING);
                    bool isFriendly = (c == WHITE) ? isWhitePiece : !isWhitePiece;
                    if(isFriendly) break;
                }
                
                //Add Valid Move:
                Move move;
                move.from = sq;
                move.to = targetSq;
                move.capturedPiece = targetPiece;
                move.promotionPiece = EMPTY;
                move.isCastling = false;
                move.isEnPassant = false;
                moves.push_back(move);

                // If it's a capture break
                if (targetPiece != EMPTY) break;

                currSq = targetSq;
            }
        }
    }
}
