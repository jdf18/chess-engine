#ifndef PIECES_H
#define PIECES_H

#include <optional>
#include "bitboard.h"

typedef enum {
    COL_BLACK,
    COL_WHITE,
    COL_NONE
} Colour;

typedef enum {
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_ROOK,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_PAWN,
    PIECE_NONE
} Pieces;

typedef struct SquareName {
    char file; char rank;
} SquareName;

typedef struct SquarePosition {
    uint8_t row;
    uint8_t column;

    SquarePosition() : row(255), column(255) {};
    SquarePosition(uint8_t row, uint8_t column) : row(row), column(column) {};

    bool is_not_defined() const {return (row == 255) && (column == 255);};

    BitBoard get_bitboard_mask() const {
        return static_cast<uint64_t>(0x1) << row + 8 * column;
    }
    SquareName get_square_name() const {
        return SquareName{
            static_cast<char>('a' + column),
            static_cast<char>('0' + row)
        };
    }
} SquarePosition;

typedef struct Move {
    SquarePosition old_position;
    SquarePosition new_position;
    std::optional<Pieces> piece_promotion; // Optional

    Move(const Move& copy) {
        old_position = copy.old_position;
        new_position = copy.new_position;
        piece_promotion = copy.piece_promotion;
    }
    Move(SquarePosition from, SquarePosition to) : old_position(from), new_position(to), piece_promotion({}) {};
    Move(SquarePosition from, SquarePosition to, Pieces promotion) :
        old_position(from), new_position(to), piece_promotion(promotion) {};
} Move;

#endif //PIECES_H