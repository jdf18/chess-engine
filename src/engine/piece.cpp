#include <bit>
#include "piece.h"
#include "boardstate.h"
#include "bitboard.h"


std::vector<Move> Piece::generateMoves(const BoardState& boardState, SquarePosition position) {
    std::vector<Move> possible_moves;
    Pieces type;

    //Get the square position as a bitboard
    BitBoard square_board = position.get_bitboard_mask();
    
    //Get the type of the piece. If the position does not contain a piece, it will assume that it's a pawn.
    if ((boardState.pieces_kings & square_board) != 0) {
        type = PIECE_KING;
    }
    else if ((boardState.pieces_queens & square_board) != 0) {
        type = PIECE_QUEEN;
    }
    else if ((boardState.pieces_rooks & square_board) != 0) {
        type = PIECE_ROOK;
    }
    else if ((boardState.pieces_bishops & square_board) != 0) {
        type = PIECE_BISHOP;
    }
    else if ((boardState.pieces_knights & square_board) != 0) {
        type = PIECE_KNIGHT;
    }
    else {
        type = PIECE_PAWN;
    }

    //Get the squares as a bitboard that we can move to.
    BitBoard move_squares = 0;
    switch (type) {
        case PIECE_KING:
            move_squares = boardState.pseudo_legal_king_moves(square_board);
            break;
        case PIECE_QUEEN:
            move_squares = boardState.pseudo_legal_queen_moves(square_board);
            break;
        case PIECE_ROOK:
            move_squares = boardState.pseudo_legal_rook_moves(square_board);
            break;
        case PIECE_BISHOP:
            move_squares = boardState.pseudo_legal_bishop_moves(square_board);
            break;
        case PIECE_KNIGHT:
            move_squares = boardState.pseudo_legal_knight_moves(square_board);
            break;
        case PIECE_PAWN:
            move_squares = boardState.pseudo_legal_pawn_moves(square_board);
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
