#ifndef PIECE_H
#define PIECE_H

#include <vector>

#include "pieces.h"
#include "boardstate.h"

class Piece {
protected:
    Colour color;
    Pieces type;

public:
    Piece(Colour colour, Pieces type) : color(colour), type(type) {};

    virtual ~Piece() = default;

    virtual std::vector<Move> generateMoves(BoardState boardState, SquarePosition position);
};

class King : public Piece {
public:
    King(Colour colour) : Piece(colour, PIECE_KING) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class Queen : public Piece {
public:
    Queen(Colour colour) : Piece(colour, PIECE_QUEEN) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class Rook : public Piece {
public:
    Rook(Colour colour) : Piece(colour, PIECE_ROOK) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class Bishop : public Piece {
public:
    Bishop(Colour colour) : Piece(colour, PIECE_BISHOP) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class Knight : public Piece {
public:
    Knight(Colour colour) : Piece(colour, PIECE_KNIGHT) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class Pawn : public Piece {
public:
    Pawn(Colour colour) : Piece(colour, PIECE_PAWN) {};
    std::vector<Move> generateMoves(BoardState boardState, SquarePosition position) override;
};

class PieceInstance {
public:
    Piece piece;
    SquarePosition position;

    std::vector<Move> generateMoves(BoardState boardState) {
        // Calls the virtual function for the specific type of piece
        return piece.generateMoves(boardState, position);
    };
};

#endif //PIECE_H
