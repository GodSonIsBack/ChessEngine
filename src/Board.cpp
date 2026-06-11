#include "../include/Board.hpp"
#include "../include/Definitions.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cctype>

Board::Board(): 
    sideToMove(WHITE),
    castLingRight(0),
    enPassantSquare(NO_SQ)
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