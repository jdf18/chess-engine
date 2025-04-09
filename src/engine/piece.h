#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <memory>

#include "pieces.h"

struct BoardState;

class Piece {
public:
    const Colour colour;
    const Pieces type;

    Piece(const Colour colour, const Pieces type) : colour(colour), type(type) {};
    ~Piece() = default;

    std::vector<Move> generateMoves(const BoardState& board_state, const SquarePosition position) const;
};

class PieceInstance {
public:
    std::unique_ptr<Piece> piece{};
    SquarePosition position{};

    PieceInstance() : piece(std::make_unique<Piece>(COL_WHITE, PIECE_PAWN)) {};
    PieceInstance(const PieceInstance& copy) : piece(std::make_unique<Piece>(copy.piece->colour, copy.piece->type)) {};
    PieceInstance(std::unique_ptr<Piece> piece, const SquarePosition position) : piece(std::move(piece)), position(position) {};

    std::vector<Move> generateMoves(const BoardState& boardState) {
        // Calls the virtual function for the specific type of piece
        return piece->generateMoves(boardState, position);
    };
};

#endif //PIECE_H
