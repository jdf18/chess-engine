#include "boardstate.h"

#define CASTLE_KINGSIDE_COL_DELTA (-2)
#define CASTLE_QUEENSIDE_COL_DELTA 3

bool BoardState::is_move_castle_valid(const Move& move) const {
    // Test to see if piece being moved is a king
    if ((pieces_kings & move.old_position.get_bitboard_mask()).board == 0) return false;

    // if end square correct and satisfies castle rights
    if (move.new_position.row != move.old_position.row) return false;

    // Find out the colour of the piece being moved
    Colour moving_colour = COL_NONE;
    if ((pieces_white & move.old_position.get_bitboard_mask()).board != 0) moving_colour = COL_WHITE;
    else if ((pieces_black & move.old_position.get_bitboard_mask()).board != 0) moving_colour = COL_BLACK;

    const uint8_t delta = abs(move.new_position.column - move.old_position.column);
    if (delta == 0) std::cout << "Invalid move (moving to same position)";

    BitBoard mask;
    if (delta == CASTLE_KINGSIDE_COL_DELTA) {
        mask = SECOND_FILE | THIRD_FILE;
    } else if (delta == CASTLE_KINGSIDE_COL_DELTA) {
        mask = FIFTH_FILE | SIXTH_FILE | SEVENTH_FILE;
    } else return false;
    if (moving_colour == COL_BLACK) mask = mask << BOARD_ROW*7;
    if (((pieces_white | pieces_black) & mask).board != 0) return false;

    if (moving_colour == COL_WHITE && white_to_move) {
        if (castling_state.white_kingside  && (delta == CASTLE_KINGSIDE_COL_DELTA))  return true;
        if (castling_state.white_queenside && (delta == CASTLE_QUEENSIDE_COL_DELTA)) return true;
    } else if (moving_colour == COL_BLACK && !white_to_move) {
        if (castling_state.black_kingside  && (delta == CASTLE_KINGSIDE_COL_DELTA))  return true;
        if (castling_state.black_queenside && (delta == CASTLE_QUEENSIDE_COL_DELTA)) return true;
    }

    return false;
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

    // todo: pawn promotion - check if pawn new position on final rank and modify moves

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

inline Colour get_castle_colour(const CastleType move) {
    return ((move == CASTLE_WHITE_KINGSIDE) || (move == CASTLE_WHITE_QUEENSIDE) ? COL_WHITE : COL_BLACK);
}

inline CastleSide get_castle_side(const CastleType move) {
    return ((move == CASTLE_WHITE_KINGSIDE) || (move == CASTLE_BLACK_KINGSIDE) ? CASTLE_KINGSIDE : CASTLE_QUEENSIDE);
}

BitBoard get_castle_xor_mask(const CastleType move) {
    switch (move) {
        case CASTLE_WHITE_KINGSIDE:
            return FIRST_RANK & (FIRST_FILE | SECOND_FILE | THIRD_FILE | FOURTH_FILE);
        case CASTLE_WHITE_QUEENSIDE:
            return FIRST_RANK & (FOURTH_FILE | FIFTH_FILE | SIXTH_FILE | EIGHTH_FILE);
        case CASTLE_BLACK_KINGSIDE:
            return EIGHTH_RANK & (FIRST_FILE | SECOND_FILE | THIRD_FILE | FOURTH_FILE);
        case CASTLE_BLACK_QUEENSIDE:
            return EIGHTH_RANK & (FOURTH_FILE | FIFTH_FILE | SIXTH_FILE | EIGHTH_FILE);
    }
    return 0;
}

bool BoardState::move_castle(const CastleType move) {
    const Colour move_colour = get_castle_colour(move);
    const CastleSide side = get_castle_side(move);

    PieceInstance* moving_king = get_piece_ptr((move_colour == COL_WHITE ? 0 : 7), 3);
    PieceInstance* moving_rook = get_piece_ptr((move_colour == COL_WHITE ? 0 : 7), (side == CASTLE_KINGSIDE ? 0 : 7));

    moving_king->position.column = (side == CASTLE_KINGSIDE ? 1 : 5);
    moving_rook->position.column = (side == CASTLE_KINGSIDE ? 2 : 4);

    const BitBoard mask = (move_colour == COL_WHITE ? FIRST_RANK : EIGHTH_RANK);

    if (move_colour == COL_WHITE) pieces_white ^= mask & (side == CASTLE_KINGSIDE ?
        (FIRST_FILE | SECOND_FILE | THIRD_FILE | FOURTH_FILE) : (FOURTH_FILE | FIFTH_FILE | SIXTH_FILE | EIGHTH_FILE));
    else pieces_black ^= mask & (side == CASTLE_KINGSIDE ?
        (FIRST_FILE | SECOND_FILE | THIRD_FILE | FOURTH_FILE) : (FOURTH_FILE | FIFTH_FILE | SIXTH_FILE | EIGHTH_FILE));

    pieces_kings ^= mask & (FOURTH_FILE | (side == CASTLE_KINGSIDE ? SECOND_FILE : SIXTH_FILE));
    pieces_rooks ^= mask & (side == CASTLE_KINGSIDE ? FIRST_FILE | THIRD_FILE : FIFTH_FILE | EIGHTH_FILE);

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
