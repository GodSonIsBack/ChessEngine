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
        for (int rank = 7; rank >= 0; rank--)
        {
            for (int file = 0; file < 8; file++)
            {
                std::cout << pieceToChar(board[rank * 8 + file]) << " ";
            }
            std::cout<<std::endl;
        }
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