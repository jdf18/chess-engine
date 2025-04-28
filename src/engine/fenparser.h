#ifndef FENPARSER_H
#define FENPARSER_H

#include "boardstate.h"
#include "piece.h"

struct PieceColour {
    Pieces piece;
    Colour colour;
};

typedef struct {
    PieceColour pieces[8];
} FenRank;

typedef struct {
    FenRank ranks[8];
    bool is_white_to_move;
    CastlingAvailability castling;
    std::optional<Move> previous_move; // Only present if en passant, worked out from en passant target square
    uint8_t halfmove_clock;
    uint16_t fullmove_number;
} FenState;

bool fen_parser(const std::string& fen, FenState& state);

#endif //FENPARSER_H
