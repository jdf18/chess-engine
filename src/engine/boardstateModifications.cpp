#include "boardstate.h"

bool BoardState::is_move_castle_valid(const Move& move, const CastleType castle) const {
    const Colour moving_colour = castle.colour;
    const CastleSide castle_side = castle.side;
    // Test to see if piece being moved is a king
    if ((pieces_kings & move.old_position.get_bitboard_mask()).board == 0) return false;

    // if end square correct row
    if (move.new_position.row != move.old_position.row) return false;

    BitBoard mask;
    if (castle_side == CASTLE_KINGSIDE) {
        mask = SIXTH_FILE | SEVENTH_FILE;
    } else if (castle_side == CASTLE_QUEENSIDE) {
        mask = SECOND_FILE | THIRD_FILE | FOURTH_FILE;
    } else return false;

    switch (moving_colour) {
        case COL_WHITE: mask &= FIRST_RANK; break;
        case COL_BLACK: mask &= EIGHTH_RANK; break;
        default: return false;
    }

    if (((pieces_white | pieces_black) & mask).board != 0) return false;

    return castling_state.castle_possible(castle);
}

// ? Possibly not required
// bool BoardState::is_move_en_passant_valid(const Move& move) const {
//     // Test to see if piece being moved is a pawn
//     if ((pieces_pawns & move.old_position.get_bitboard_mask()).board == 0) return false;
//
//     // if new square empty
//     if (((pieces_white | pieces_black) & move.old_position.get_bitboard_mask()).board == 0) return false;
//
//     // if diagonally forwards
//     // if legal from last move
//     return false;
// }

PieceInstance BoardState::get_piece(const SquarePosition position) const {
    return get_piece(position.row, position.column);
}
PieceInstance BoardState::get_piece(const uint8_t row, const uint8_t column) const {
    const BitBoard square_mask = SquarePosition{row, column}.get_bitboard_mask();
    Colour piece_colour;

    if ((pieces_white & square_mask).board != 0) {
        piece_colour = COL_WHITE;
    } else if ((pieces_black & square_mask).board != 0) {
        piece_colour = COL_BLACK;
    } else {
        return PieceInstance{std::make_unique<Piece>(COL_NONE, PIECE_NONE), SquarePosition{row, column}};
    }

    if ((pieces_kings & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_KING), SquarePosition{row, column}};
    }
    if ((pieces_queens & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_QUEEN), SquarePosition{row, column}};
    }
    if ((pieces_rooks & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_ROOK), SquarePosition{row, column}};
    }
    if ((pieces_bishops & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_BISHOP), SquarePosition{row, column}};
    }
    if ((pieces_knights & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_KNIGHT), SquarePosition{row, column}};
    }
    if ((pieces_pawns & square_mask).board != 0) {
        return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_PAWN), SquarePosition{row, column}};
    }

    return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_NONE), SquarePosition{row, column}};
}

PieceInstance* BoardState::get_piece_ptr(const uint8_t row, const uint8_t column) {
    for (int i = 0; i < PIECE_ARRAY_BLACK_OFFSET*2; i++) {
        PieceInstance* piece = &pieces[i];
        if (piece->position.row == row && piece->position.column == column) {
            return piece;
        }
    }
    return nullptr;
}


bool BoardState::move_piece(const SquarePosition start_position, const SquarePosition end_position) {
    PieceInstance* moving_piece = get_piece_ptr(start_position.row, start_position.column);
    if ((moving_piece->piece->colour == COL_NONE) && (moving_piece->piece->type == PIECE_NONE)) return false;

    // Delete the taken piece (iff it exists)
    PieceInstance* taken_piece = get_piece_ptr(end_position.row, end_position.column);
    if (taken_piece != nullptr) {
        // if taking a piece, delete piece in pieces array
        taken_piece->piece->colour = COL_NONE;
        taken_piece->piece->type = PIECE_NONE;
    }

    // move the piece to the new position
    moving_piece->position = end_position;

    // Change the positions of the pieces in the bitboards
    const BitBoard move_mask = (start_position.get_bitboard_mask() | end_position.get_bitboard_mask());
    if (moving_piece->piece->colour == COL_WHITE) {
        // Toggle the position of the piece being moved
        pieces_white ^= move_mask;
        // Ensure no pieces from the opposing side are on the move squares
        pieces_black &= ~move_mask;
    } else if (moving_piece->piece->colour == COL_BLACK) {
        pieces_black ^= move_mask;
        pieces_white &= ~move_mask;
    } else {
        return false;
    }

    // Remove all pieces (types) from both squares involved with the move
    pieces_kings   &= ~move_mask;
    pieces_queens  &= ~move_mask;
    pieces_rooks   &= ~move_mask;
    pieces_bishops &= ~move_mask;
    pieces_knights &= ~move_mask;
    pieces_pawns   &= ~move_mask;

    // Add back the moved piece (type) onto the end square
    switch (moving_piece->piece->type) {
        case PIECE_KING:
            pieces_kings |= end_position.get_bitboard_mask();
            break;
        case PIECE_QUEEN:
            pieces_queens |= end_position.get_bitboard_mask();
            break;
        case PIECE_ROOK:
            pieces_rooks |= end_position.get_bitboard_mask();
            break;
        case PIECE_BISHOP:
            pieces_bishops |= end_position.get_bitboard_mask();
            break;
        case PIECE_KNIGHT:
            pieces_knights |= end_position.get_bitboard_mask();
            break;
        case PIECE_PAWN:
            pieces_pawns |= end_position.get_bitboard_mask();
            break;
        default: break;
    }

    return true;
}

BitBoard get_castle_xor_mask(const CastleType move) {
    BitBoard mask;
    if (move.side == CASTLE_KINGSIDE) {
        mask = (FIRST_FILE | SECOND_FILE | THIRD_FILE | FOURTH_FILE);
    } else {
        mask = (FOURTH_FILE | FIFTH_FILE | SIXTH_FILE | EIGHTH_FILE);
    }

    if (move.colour == COL_WHITE) {
        return mask & FIRST_RANK;
    }
    return mask & EIGHTH_RANK;
}

bool BoardState::move_castle(const CastleType move) {
    const Colour move_colour = move.colour;
    const CastleSide side = move.side;

    // move piece objects
    PieceInstance* moving_king = get_piece_ptr((move_colour == COL_WHITE ? 0 : 7), 4);
    PieceInstance* moving_rook = get_piece_ptr((move_colour == COL_WHITE ? 0 : 7), (side == CASTLE_KINGSIDE ? 0 : 7));

    moving_king->position = SquarePosition{moving_king->position.row, static_cast<unsigned char>(side == CASTLE_KINGSIDE ? 1 : 5)};
    moving_rook->position = SquarePosition{moving_king->position.row, static_cast<unsigned char>(side == CASTLE_KINGSIDE ? 2 : 4)};

    // change bitboards
    const BitBoard colour_mask = (move_colour == COL_WHITE ? FIRST_RANK : EIGHTH_RANK);
    const BitBoard kings_mask = (FIFTH_FILE | (side == CASTLE_KINGSIDE ? THIRD_FILE : SEVENTH_FILE));
    const BitBoard rooks_mask = (side == CASTLE_KINGSIDE ? FIRST_FILE | FOURTH_FILE : SIXTH_FILE | EIGHTH_FILE);

    if (move_colour == COL_WHITE) pieces_white ^= colour_mask & (kings_mask | rooks_mask);
    else pieces_black ^= colour_mask & (kings_mask | rooks_mask);

    pieces_kings ^= colour_mask & kings_mask;
    pieces_rooks ^= colour_mask & rooks_mask;

    return true;
}

bool BoardState::remove_piece(const SquarePosition position) {
    const PieceInstance* taken_piece = get_piece_ptr(position.column, position.row);
    taken_piece->piece->colour = COL_NONE;
    taken_piece->piece->type = PIECE_NONE;

    const BitBoard mask = position.get_bitboard_mask();

    pieces_white &= ~mask;
    pieces_black &= ~mask;

    pieces_kings   &= ~mask;
    pieces_queens  &= ~mask;
    pieces_rooks   &= ~mask;
    pieces_bishops &= ~mask;
    pieces_knights &= ~mask;
    pieces_pawns   &= ~mask;

    return true;
}

void BoardState::switch_turn() {
    white_to_move = !white_to_move;
}


void BoardState::promote_piece(const SquarePosition position, const Pieces type) {
    const BitBoard mask = position.get_bitboard_mask();
    pieces_pawns &= ~mask;

    switch (type) {
        case PIECE_KING: pieces_kings |= mask; break;
        case PIECE_QUEEN: pieces_queens |= mask; break;
        case PIECE_ROOK: pieces_rooks |= mask; break;
        case PIECE_BISHOP: pieces_bishops |= mask; break;
        case PIECE_KNIGHT: pieces_knights |= mask; break;
        case PIECE_PAWN: pieces_pawns |= mask; break;
        default: break;
    }

    return;
}