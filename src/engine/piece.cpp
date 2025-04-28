#include <bit>
#include "piece.h"
#include "boardstate.h"
#include "bitboard.h"


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
    const int8_t direction_row, const int8_t direction_column, const Colour colour) {
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
    Pieces type;

    //Get the square position as a bitboard
    BitBoard square_board = position.get_bitboard_mask();
    
    //Get the type of the piece. If the position does not contain a piece, it will assume that it's a pawn.
    if ((board_state->pieces_kings & square_board) != 0) {
        type = PIECE_KING;
    }
    else if ((board_state->pieces_queens & square_board) != 0) {
        type = PIECE_QUEEN;
    }
    else if ((board_state->pieces_rooks & square_board) != 0) {
        type = PIECE_ROOK;
    }
    else if ((board_state->pieces_bishops & square_board) != 0) {
        type = PIECE_BISHOP;
    }
    else if ((board_state->pieces_knights & square_board) != 0) {
        type = PIECE_KNIGHT;
    }
    else {
        type = PIECE_PAWN;
    }

    //Get the squares as a bitboard that we can move to.
    BitBoard move_squares = 0;
    switch (type) {
        case PIECE_KING:
            move_squares = board_state->pseudo_legal_king_moves(square_board);
            break;
        case PIECE_QUEEN:
            move_squares = board_state->pseudo_legal_queen_moves(square_board);
            break;
        case PIECE_ROOK:
            move_squares = board_state->pseudo_legal_rook_moves(square_board);
            break;
        case PIECE_BISHOP:
            move_squares = board_state->pseudo_legal_bishop_moves(square_board);
            break;
        case PIECE_KNIGHT:
            move_squares = board_state->pseudo_legal_knight_moves(square_board);
            break;
        case PIECE_PAWN:
            move_squares = board_state->pseudo_legal_pawn_moves(square_board);
            break;
    }

    //Brian Kernighan's method for counting set bits, found here: https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
    uint64_t v = move_squares.board; // count the number of bits set in v
    uint64_t c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
    {
        v &= v - 1; // clear the least significant bit set
    }

    //Reserve the number of moves we will have. This saves time resizing the vector.
    possible_moves.reserve(c);
    

    //For each move (while move squares != 0), we get the rightmost move, add a move from the position to that square, and remove it from move squares.
    while (move_squares != 0) {
        uint64_t rightmost_move = static_cast<uint64_t>(0x1) << std::countr_zero(move_squares.board);
        move_squares ^= rightmost_move;
        SquarePosition destination = SquarePosition(rightmost_move);
        possible_moves.push_back(Move(position, destination));
    }

    return possible_moves;
}
