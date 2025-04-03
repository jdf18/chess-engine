#include "piece.h"

#include "boardstate.h"

#define IS_ON_BOARD(pos, row_offset, col_offset) (0 <= pos.row + row_offset <= 7) && (0 <= pos.column + col_offset <= 7)

void conditionally_add_position_offset_to_moves_vector(
    const BoardState& board_state, const SquarePosition position, std::vector<Move>& moves,
    const uint8_t row_offset, const uint8_t column_offset) {

    if (IS_ON_BOARD(position, row_offset, column_offset)) {
        const SquarePosition position_after_move = {
            static_cast<uint8_t>(position.row + row_offset),
            static_cast<uint8_t>(position.column + column_offset)
        };
        // todo: check no friendly pieces on test_position
        // Not currently checking for checks as will first check based on if king can be taken next move
        // Not checking for king is fine as equivalent to being taken next move

        const Move move {position, position_after_move};
        moves.push_back(move);
    }
}

std::vector<Move> Piece::generateMoves(const BoardState& boardState, const SquarePosition position) const {
    std::vector<Move> possible_moves;

    switch (type) {
        case PIECE_KING:
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 1, 1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 1, 0);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 1, -1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 0, 1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 0, -1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -1, 1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -1, 0);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -1, -1);
            break;
        case PIECE_KNIGHT:
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 1, 2);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 1, -2);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -1, 2);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -1, -2);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 2, 1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, 2, -1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -2, 1);
            conditionally_add_position_offset_to_moves_vector(
                boardState, position, possible_moves, -2, -1);
            break;
        case PIECE_QUEEN:
        case PIECE_ROOK:
        case PIECE_BISHOP:
            break;
        case PIECE_PAWN:
            break;
    }
    // todo: implementation here
    return possible_moves;
}
