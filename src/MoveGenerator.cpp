#include <vector>

#include "../include/MoveGenerator.hpp"
#include "../include/Board.hpp"
#include "../include/Move.hpp"
#include "../include/Definitions.hpp"

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
            moves.push_back(Move((Square)sq,(Square)targetSq,targetPiece));
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
            moves.push_back(Move((Square)sq,(Square)targetSq,targetPiece));
        }

        if(c == WHITE && sq == E1)
        {
            //White Kingside
                //if king is on E1 and F1 and G1 are empty
                if((board.castLingRight & 1) && 
                    board.getPiece(F1) == EMPTY && board.getPiece(G1) == EMPTY)
                {
                    moves.push_back(Move(E1, G1, EMPTY, EMPTY, true));
                }
            
            //White Queenside
                //if king is on E1 and B1, C1 and D1 are empty
                if((board.castLingRight & 2) && board.getPiece(B1) == EMPTY && 
                    board.getPiece(C1) == EMPTY && board.getPiece(D1) == EMPTY)
                {
                    moves.push_back(Move(E1, C1, EMPTY, EMPTY, true));
                }
        }
        else if (c == BLACK && sq == E8)
        {
            //Black Kingside
                //if king is on E8 and F8 and G8 are empty
                if((board.castLingRight & 4) && board.getPiece(F8) == EMPTY && 
                    board.getPiece(G8) == EMPTY)
                {
                    moves.push_back(Move(E8, G8, EMPTY, EMPTY, true));
                }
            
            //Black Queenside
                //if king is on E8 and B8, C8 and D8 are empty
                if((board.castLingRight & 8) && board.getPiece(C8) == EMPTY && 
                    board.getPiece(B8) == EMPTY && board.getPiece(D8) == EMPTY)
                {
                    moves.push_back(Move(E8, C8, EMPTY, EMPTY, true));
                }
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
                moves.push_back(Move((Square)sq,(Square)targetSq,targetPiece));

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
                if(file == 7 && (offset == 9 || offset == -7)) break;
                if(file == 0 && (offset == 7 || offset == -9)) break;

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
                moves.push_back(Move((Square)sq,(Square)targetSq,targetPiece));

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
                if(file == 7 && (offset == 9 || offset == -7 || offset == 1)) break;
                if(file == 0 && (offset == 7 || offset == -9 || offset == -1)) break;

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
                moves.push_back(Move((Square)sq,(Square)targetSq,targetPiece));  

                // If it's a capture break
                if (targetPiece != EMPTY) break;

                currSq = targetSq;
            }
        }
    }
}

void MoveGenerator::genPawnMoves(Board &board, std::vector<Move> &moves)
{
    Color c = board.getSideToMove();
    Piece PAWN = (c == WHITE) ? W_PAWN : B_PAWN;

    auto addPawnMove = [&](Square fromSq, Square toSq, Piece captured) {
        int promoRank = (c == WHITE) ? 7 : 0;
        
        // If the pawn landed on the final rank, it promotes
        if (toSq / 8 == promoRank) {
            moves.push_back(Move(fromSq, toSq, captured, (c == WHITE) ? W_QUEEN : B_QUEEN));
            moves.push_back(Move(fromSq, toSq, captured, (c == WHITE) ? W_ROOK : B_ROOK));
            moves.push_back(Move(fromSq, toSq, captured, (c == WHITE) ? W_BISHOP : B_BISHOP));
            moves.push_back(Move(fromSq, toSq, captured, (c == WHITE) ? W_KNIGHT : B_KNIGHT));
        } else {
            // Normal move
            moves.push_back(Move((Square)fromSq, (Square)toSq, captured, EMPTY));
        }
    };
    
    for(int sq = 0;sq < 64; sq++)
    {
        if(board.getPiece(sq) != PAWN) continue;

        int rank = sq / 8;
        int file = sq % 8;

        //White Pawn:
        if(PAWN == W_PAWN)
        {
            // ------ FORWARD MOVEMENTS --------
            int targetSq = sq + 8;
    
            if (targetSq <= 63 && board.getPiece(targetSq) == EMPTY) 
            {
                // Single step
                addPawnMove((Square)sq, (Square)targetSq, EMPTY);

                // Double Step 
                if (rank == 1) 
                {
                    int doubleSq = sq + 16;

                    if (board.getPiece(doubleSq) == EMPTY) 
                    {
                        addPawnMove((Square)sq, (Square)doubleSq, EMPTY);
                    }
                }
            }

            // ------- DIAGONAL CAPTURES -------
            int offsets[] = {7, 9};

            for(auto offset : offsets)
            {
                int targetSq = sq + offset;
                
                //Wrap-around
                if(file == 7 && offset == 9) continue;
                if(file == 0 && offset == 7) continue;

                //Out-of-Bounds
                if(targetSq < 0 || targetSq > 63) continue;

                Piece targetPiece = board.getPiece(targetSq);

                if(targetPiece >= B_PAWN && targetPiece <= B_KING)
                {
                    addPawnMove((Square)sq, (Square)targetSq, targetPiece);
                }
                // ---EN PASSANT---
                else if (targetSq == board.getEnPassantSquare())
                {
                    // Move to empty quare but capture black pawn
                    Move epMove((Square)sq, (Square)targetSq, B_PAWN);
                    epMove.isEnPassant = true;
                    moves.push_back(epMove);
                }
            }
        }
        //Black Pawn:
        else
        {
            // ------ FORWARD MOVEMENTS --------
            int targetSq = sq - 8;
    
            if (targetSq >= 0 && board.getPiece(targetSq) == EMPTY) 
            {
                // Single step
                addPawnMove((Square)sq, (Square)targetSq, EMPTY);

                // Double Step 
                if (rank == 6) 
                {
                    int doubleSq = sq - 16;

                    if (board.getPiece(doubleSq) == EMPTY) 
                    {
                        addPawnMove(
                            (Square)sq, (Square)doubleSq, EMPTY
                        );
                    }
                }
            }

            // ------- DIAGONAL CAPTURES -------
            int offsets[] = {-7, -9};

            for(auto offset : offsets)
            {
                int targetSq = sq + offset;
                
                //Wrap-around
                if(file == 7 && offset == -7) continue;
                if(file == 0 && offset == -9) continue;

                //Out-of-Bounds
                if(targetSq < 0 || targetSq > 63) continue;

                Piece targetPiece = board.getPiece(targetSq);

                if(targetPiece >= W_PAWN && targetPiece <= W_KING)
                {
                    addPawnMove(
                        (Square)sq, (Square)targetSq, targetPiece
                    );
                }
                // ---EN PASSANT---
                else if (targetSq == board.getEnPassantSquare())
                {
                    // Move to empty quare but capture white pawn
                    moves.push_back(
                        Move((Square)sq, (Square)targetSq, W_PAWN, EMPTY, false, true)
                    );
                }
            }
        }
    }
}


std::vector<Move> MoveGenerator::generateAllMoves(Board &board)
{
    std::vector<Move> moves;
    moves.reserve(256);
    
    // generate moves for all pieces:
    genKnightMoves(board,moves);
    genKingMoves(board,moves);
    genRookMoves(board,moves);
    genBishopMoves(board,moves);
    genQueenMoves(board,moves);
    genPawnMoves(board,moves);

    return moves;
}