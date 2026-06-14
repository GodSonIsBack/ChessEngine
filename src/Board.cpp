#include "../include/Board.hpp"
#include "../include/Definitions.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cctype>

Board::Board(): 
    sideToMove(WHITE),
    castLingRight(0),
    enPassantSquare(NO_SQ),
    histPtr(0)
    {
        loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }

// ----------PUBLIC FUNCTIONS----------
    void Board::setPiece(Square s, Piece p)
    {
        board[s] = p;
    }

    void Board::printBoard()
    {
        std::cout<<std::endl;
        for (int rank = 7; rank >= 0; rank--)
        {
            std::cout<<rank+1<<" | ";
            for (int file = 0; file < 8; file++)
            {
                std::cout << pieceToChar(board[rank * 8 + file]) << " ";
            }
            std::cout<<std::endl;
        }
        std::cout<<"    ---------------"<<std::endl;
        std::cout<<"    a b c d e f g h"<<std::endl;
        std::cout<<std::endl;
    }

    void Board::loadFEN(std::string fen)
    {
        std::stringstream ss(fen);

        // separating all FEN fields:
        ChessPosition pos;
        ss  >> pos.piecePlacement 
            >> pos.activeColor 
            >> pos.castLingRights 
            >> pos.enPassantSquare 
            >> pos.halfMoveClock 
            >> pos.fullMoveCounter;
        
        // Clear the board first
        for (auto &piece : board)
        {
            piece = EMPTY;
        }

        // loading pieces onto the board:
        std::stringstream rowStream(pos.piecePlacement);
        std::string row;

        int rank = 7; // since row is from top to bottom
        int file = 0;

        while (std::getline(rowStream, row, '/'))
        {
            for (char c : row)
            {
                if (std::isdigit(c))
                {
                    file += c - '0';
                }
                else
                {
                    Square sq = (Square)(rank * 8 + file);

                    //Store cordinates of Kings:
                    if(c == 'K') King_W = sq;
                    if(c == 'k') King_B = sq;
                    
                    setPiece(sq, charToPiece(c));
                    file++;
                }
            }
            rank--;
            file = 0;
        }

        // parse active color:
        sideToMove = (pos.activeColor == 'w') ? WHITE : BLACK;

        //parse Castling Rights:
        castLingRight = 0;
        for (char c : pos.castLingRights)
        {
            switch (c)
            {
                case 'K': castLingRight |= 1; break;
                case 'Q': castLingRight |= 2; break;
                case 'k': castLingRight |= 4; break;
                case 'q': castLingRight |= 8; break;
                default: break;
            }
        }

        //parse enPassantSquare
        if (pos.enPassantSquare == "-") 
        {
            enPassantSquare = NO_SQ;
        } 
        else 
        {
            int enp_file = pos.enPassantSquare[0] - 'a';
            int enp_rank = pos.enPassantSquare[1] - '1';
            enPassantSquare = (Square)(enp_rank * 8 + enp_file);
        }
    }

    void Board::makeMove(Move move)
    {
        Square from = move.from;
        Square to = move.to;
        Piece movedPiece = board[from];
        
        // Snapshot current Board State:
            history[histPtr].castLingRight = castLingRight;
            history[histPtr].enPassantSquare = enPassantSquare;
            histPtr++;

        // CastlingRight Update:
            static const int castlingRightsArray[64] = {
            13, 15, 15, 15, 12, 15, 15, 14,  // Rank 1 (White)
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            7, 15, 15, 15,  3, 15, 15, 11   // Rank 8 (Black)
            };

            castLingRight &= castlingRightsArray[from]; // if King or Rook is moved
            castLingRight &= castlingRightsArray[to]; // if Rook is Taken

            // Moving Rook:
            if(move.isCastling)
            {
                switch(movedPiece)
                {
                    case W_KING: 
                        switch(to)
                        {
                            case G1: 
                                board[F1] = board[H1]; board[H1] = EMPTY; break;
                            case C1:
                                board[D1] = board[A1]; board[A1] = EMPTY; break;
                        }
                        break;
                    case B_KING:
                        switch(to)
                        {
                            case G8: 
                                board[F8] = board[H8]; board[H8] = EMPTY; break;
                            case C8:
                                board[D8] = board[A8]; board[A8] = EMPTY; break;
                        }
                        break;
                } 
            }

        // EnPassant Update:
            if (move.isEnPassant) 
            {
                int capturedPawnSq = (sideToMove == WHITE) ? to - 8 : to + 8;
                board[capturedPawnSq] = EMPTY;
            }

            // wipes the enPassant square every move(cuz enpassant lasts for only one move): 
            enPassantSquare = NO_SQ;

            if ((movedPiece == W_PAWN || movedPiece == B_PAWN) && 
                (to - from == 16 || from - to == 16)) 
            {   
                // The En Passant square is perfectly halfway between 'from' and 'to'!
                enPassantSquare = (Square)((from + to) / 2);
            }

        // Piece Movement:
            board[to] = board[from];
            board[from] = EMPTY;
            if(movedPiece == W_KING) King_W = to;
            if(movedPiece == B_KING) King_B = to;
        
        // Pawn Promotion:
            if(move.promotionPiece != EMPTY) board[to] = move.promotionPiece;

        // Change the turn:
            sideToMove = (Color)(sideToMove ^ 1);
    }

    void Board::unmakeMove(Move move)
    {
        Square from = move.from;
        Square to = move.to;
        Piece capturedPiece = move.capturedPiece;
        Piece movedPiece = board[to];

        // Restoring prev state:
            histPtr--;
            castLingRight = history[histPtr].castLingRight;
            enPassantSquare = history[histPtr].enPassantSquare;

        // Undo turn:
            sideToMove = (Color)(sideToMove ^ 1);

        // Undo Castling:
            if(move.isCastling)
            {
                switch(to)
                {
                    case C1:
                        board[A1] = board[D1]; board[D1] = EMPTY; break; 
                    case G1:
                        board[H1] = board[F1]; board[F1] = EMPTY; break; 
                    case C8:
                        board[A8] = board[D8]; board[D8] = EMPTY; break; 
                    case G8:
                        board[H8] = board[F8]; board[F8] = EMPTY; break; 
                }
            }

        // Undo Piece Movement:
            board[from] = board[to];
            board[to] = EMPTY;
            if(movedPiece == W_KING) King_W = from;
            if(movedPiece == B_KING) King_B = from;
        
        // Restore Captured Piece:
            if(!move.isEnPassant)
                board[to] = capturedPiece;
        
        // Undo Promotions:
            if(move.promotionPiece != EMPTY) board[from] = (sideToMove == WHITE) ? 
                                                            W_PAWN : B_PAWN;

        // Undo EnPassant:
            if(move.isEnPassant)
            {
                int capturedPawnSq = (sideToMove == WHITE) ? to - 8 : to + 8;
                board[capturedPawnSq] = capturedPiece;
            }
    }

    bool Board::isSquareAttacked(Square sq,Color attackingSide)
    {
        int file = sq % 8;
        int rank = sq / 8;

        //Knight Attack:
            Piece targetKnight = (attackingSide == WHITE) ? W_KNIGHT : B_KNIGHT;
            int offsets[] = {15, 17, -17, -15, 6, 10, -6, -10};

            for(auto offset : offsets)
            {
                int tempSq = sq + offset;

                // Wrap-around:
                if ((offset == -10 || offset == 6) && file < 2) continue;
                if ((offset == -17 || offset == 15) && file < 1) continue;
                if ((offset == -15 || offset == 17) && file > 6) continue;
                if ((offset == -6 || offset == 10) && file > 5) continue;

                // Out of Bounds:
                if(tempSq < 0 || tempSq > 63) continue;

                if(board[tempSq] == targetKnight) return true;
            }
        
        //Pawn Attack:
            if (attackingSide == WHITE)
            {
                if (file != 0 && sq - 9 >= 0 && board[sq - 9] == W_PAWN) return true;
                if (file != 7 && sq - 7 >= 0 && board[sq - 7] == W_PAWN) return true;
            }
            else
            {
                if (file != 0 && sq + 7 <= 63 && board[sq + 7] == B_PAWN) return true;
                if (file != 7 && sq + 9 <= 63 && board[sq + 9] == B_PAWN) return true;
            }
        
        //Sliding Pieces:
            // Diagonal Attacks:
            int diagOffsets[] = {7, 9, -7, -9};
            Piece enemyBishop = (attackingSide == WHITE) ? W_BISHOP : B_BISHOP;
            Piece enemyQueen  = (attackingSide == WHITE) ? W_QUEEN : B_QUEEN;

            for (int offset : diagOffsets) 
            {                
                int tempSq = sq;
                while (true) 
                {
                    int file = tempSq % 8;
                    
                    // Wrap Around
                    if ((offset == 7 || offset == -9) && file == 0) break;
                    if ((offset == 9 || offset == -7) && file == 7) break;

                    tempSq += offset;
                    
                    // Out of Bounds
                    if (tempSq < 0 || tempSq > 63) break;

                    Piece hitPiece = board[tempSq];
                    if (hitPiece != EMPTY) 
                    {
                        if (hitPiece == enemyBishop || hitPiece == enemyQueen) return true;
                        break; 
                    }
                }
            }

            // Straight Attacks:
            int straightOffsets[] = {8, -8, 1, -1};
            Piece enemyRook = (attackingSide == WHITE) ? W_ROOK : B_ROOK;

            for (int offset : straightOffsets) 
            {
                int tempSq = sq;
                while (true) 
                {
                    int file = tempSq % 8;
                    
                    // Wrap Around
                    if (offset == -1 && file == 0) break;
                    if (offset == 1 && file == 7) break;

                    tempSq += offset;
                    
                    // Out of Bounds
                    if (tempSq < 0 || tempSq > 63) break;

                    Piece hitPiece = board[tempSq];
                    if (hitPiece != EMPTY) 
                    {
                        if (hitPiece == enemyRook || hitPiece == enemyQueen) return true;
                        break; 
                    }
                }
            }
        
        //King Attack:
            int kingOffsets[]={1, -1, 8, -8, 7, -7, 9, -9};
            Piece enemyKing = (attackingSide == WHITE) ? W_KING : B_KING;
            for (int offset : kingOffsets) 
            {
                // Wrap around checks
                if (file == 0 && (offset == -1 || offset == -9 || offset == 7)) continue;
                if (file == 7 && (offset == 1 || offset == -7 || offset == 9)) continue;

                int tempSq = sq + offset;
                if (tempSq < 0 || tempSq > 63) continue;

                if (board[tempSq] == enemyKing) return true;
            }

        return false; // Square is Safe
    }

    int Board::evaluate()
    {
        int score=0;
        for(int sq=0;sq < 64;sq++)
        {
            Piece currPiece = board[sq];
            if(currPiece == EMPTY) continue;

            switch(currPiece)
            {
                case W_PAWN: score += PAWN + PawnTable[sq]; break;
                case B_PAWN: score -= PAWN + PawnTable[sq ^ 56]; break;

                case W_BISHOP: score += BISHOP + BishopTable[sq]; break;
                case B_BISHOP: score -= BISHOP + BishopTable[sq ^ 56]; break;

                case W_ROOK: score += ROOK + RookTable[sq]; break;
                case B_ROOK: score -= ROOK + RookTable[sq ^ 56]; break;

                case W_QUEEN: score += QUEEN + QueenTable[sq]; break;
                case B_QUEEN: score -= QUEEN + QueenTable[sq ^ 56]; break;

                case W_KNIGHT: score += KNIGHT + KnightTable[sq]; break;
                case B_KNIGHT: score -= KNIGHT + KnightTable[sq ^ 56]; break;

                case W_KING: score += KingTable[sq]; break;
                case B_KING: score -= KingTable[sq ^ 56]; break;
            }
        }
        return (sideToMove == WHITE) ? score : -score;
    }
// ----------GETTERS----------
    Piece Board::getPiece(Square s)
    {
        return board[s];
    }

    Color Board::getSideToMove()
    {
        return sideToMove;
    }

    Square Board::getEnPassantSquare()
    {
        return enPassantSquare;
    }

    int Board::getCastLingRight()
    {
        return castLingRight;
    }

    Square Board::getKingSq(Color playingSide)
    {
        return (playingSide == WHITE) ? King_W : King_B;
    }

// ----------HELPER FUNCTIONS----------
    Piece Board::charToPiece(char c)
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

    char Board::pieceToChar(Piece p)
    {
        const std::string pieceChars=".PNBRQKpnbrqk";
        return pieceChars[p];
    }