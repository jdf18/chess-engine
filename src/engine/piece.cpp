#include "piece.h"

#include "boardstate.h"

#define IS_ON_BOARD(pos, row_offset, col_offset) ((0 <= pos.row + row_offset && pos.row + row_offset <= 7) && (0 <= pos.column + col_offset && pos.column + col_offset <= 7))

bool conditionally_add_position_offset_to_moves_vector(
    const BoardState* board_state, const SquarePosition position, std::vector<Move>& moves,
    const int8_t row_offset, const int8_t column_offset, const Colour colour) {

    if (IS_ON_BOARD(position, row_offset, column_offset)) {
        const SquarePosition position_after_move = {
            static_cast<uint8_t>(position.row + row_offset),
            static_cast<uint8_t>(position.column + column_offset)
        };

        if (((colour==COL_WHITE ? board_state->pieces_white : board_state->pieces_black).board &
            position_after_move.get_bitboard_mask().board) != 0) {
            return false;
        }
        // todo Not currently checking for checks as will first check based on if king can be taken next move
        // Not checking for king is fine as equivalent to being taken next move

        const Move move {position, position_after_move};
        moves.push_back(move);
        return true;
    }
    return false;
}

void add_moves_to_vector_along_direction(
    const BoardState* board_state, const SquarePosition position, std::vector<Move>& moves,
    const uint8_t direction_row, const uint8_t direction_column, const Colour colour) {
    for (int i =0; i < 8; i++) {
        if (IS_ON_BOARD(position, i*direction_row, i*direction_column)) {
            if (!conditionally_add_position_offset_to_moves_vector(
                    board_state, position, moves, i*direction_row, i*direction_column, colour
                )) {
                // Either off the board or hit a friendly piece, stop the for loop
                break;
                }
            if ((colour==COL_BLACK ? board_state->pieces_white : board_state->pieces_black).board &
                (0x1 << position.row + (i*direction_row) + 8 * (position.column + (i*direction_column))) != 0) {
                // Piece is taking an enemy piece
                break;
            }
        }
    }
}

std::vector<Move> Piece::generateMoves(const BoardState* board_state, const SquarePosition position) const {
    std::vector<Move> possible_moves;

    switch (type) {
        case PIECE_KING:
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 1, 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 1, 0, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 1, -1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 0, 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 0, -1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -1, 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -1, 0, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -1, -1, colour);
            break;
        case PIECE_KNIGHT:
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 1, 2, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 1, -2, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -1, 2, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -1, -2, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 2, 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, 2, -1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -2, 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, -2, -1, colour);
            break;
        case PIECE_QUEEN:
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, 0, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, 0, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 0, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 0, -1, colour);

            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, -1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, -1, colour);
            break;
        case PIECE_ROOK:
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, 0, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, 0, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 0, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 0, -1, colour);
            break;
        case PIECE_BISHOP:
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, 1, -1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, 1, colour);
            add_moves_to_vector_along_direction(
                board_state, position, possible_moves, -1, -1, colour);
            break;
        case PIECE_PAWN:
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, (colour==COL_WHITE ? 1 : -1), 0, colour);
            // todo: add condition: on first move
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, (colour==COL_WHITE ? 2 : -2), 0, colour);
            // todo: add condition: if enemy piece OR en passant possible
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, (colour==COL_WHITE ? 1 : -1), 1, colour);
            conditionally_add_position_offset_to_moves_vector(
                board_state, position, possible_moves, (colour==COL_WHITE ? 1 : -1), -1, colour);
            break;
        default: break;
    }
    return possible_moves;
}
