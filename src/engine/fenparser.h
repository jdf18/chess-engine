#ifndef FENPARSER_H
#define FENPARSER_H

#include "boardstate.h"
#include "piece.h"
#include "castlingavailability.h"

struct PieceColour {
    Pieces piece;
    Colour colour;
};

typedef struct {
    PieceColour pieces[8];
} FenRank;

struct FenState {
    FenRank ranks[8];
    bool is_white_to_move;
    CastlingAvailability castling;
    std::optional<Move> previous_move; // Only present if en passant, worked out from en passant target square
    uint8_t halfmove_clock;
    uint16_t fullmove_number;
};

bool fen_parser(const std::string& fen, FenState& state);

#endif //FENPARSER_H
