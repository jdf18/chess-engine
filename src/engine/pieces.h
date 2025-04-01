#ifndef PIECES_H
#define PIECES_H

#include <optional>

typedef enum {
    COL_BLACK,
    COL_WHITE
} Colour;

typedef enum {
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_ROOK,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_PAWN
} Pieces;

typedef struct SquarePosition {
    uint8_t row;
    uint8_t column;

    SquarePosition() : row(255), column(255) {};
    SquarePosition(uint8_t row, uint8_t column) : row(row), column(column) {};

    bool is_not_defined() const {return (row == 255) && (column == 255);};
} SquarePosition;

typedef struct Move {
    SquarePosition old_position;
    SquarePosition new_position;
    std::optional<Pieces> piece_promotion; // Optional

    Move(SquarePosition from, SquarePosition to) : old_position(from), new_position(to), piece_promotion({}) {};
    Move(SquarePosition from, SquarePosition to, Pieces promotion) :
        old_position(from), new_position(to), piece_promotion(promotion) {};
} Move;

#endif //PIECES_H