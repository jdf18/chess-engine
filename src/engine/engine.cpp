#include <cstdint>

// bitboards: 64 bit unsigned integers, with 1 bit for each square of the board.
// There will be quite a few different bitboards, containing useful information.
// The least significant bit is the square a1. The next least significant is b1, so on and so forth. The ninth least significant will be a2.
// And the most significant is h8.
// Bitboard list:
// pieces : Has a bit where all pieces on the board are.
// pieces_w : Has a bit where all white pieces are.
// pieces_b : Has a bit where all black pieces are.
// pieces_w_K : Has a bit where all white kings are (there should only be one.)
// pieces_w_Q : Has a bit where all white queens are
// pieces_w_R : Has a bit where all white rooks are
// pieces_w_N : Has a bit where all white knights are
// pieces_w_B : Has a bit where all white bishops are.
// pieces_w_P : Has a bit where all white pawns are.
// pieces_b_K : Has a bit where all white kings are (there should only be one.)
// pieces_b_Q : Has a bit where all white queens are
// pieces_b_R : Has a bit where all white rooks are
// pieces_b_N : Has a bit where all white knights are
// pieces_b_B : Has a bit where all white bishops are.
// pieces_b_P : Has a bit where all white pawns are.

// There may be more later, but as of right now these are the ones I expect to have.
// They represent the entire state of the board, along with extra information
// such as having all pieces for one or both sides in a single bitboard.


// Returns true if there is a piece on the square (or any of the squares) given by bitboard 'square'.
// Can check if there is a specific piece or piece of a specific colour by passing that bitboard for pieces, rather than the full one.
bool piece_on_squares(uint64_t squares, uint64_t pieces) {
	return (squares & pieces) > 0
}

// Returns all the valid squares for a knight on 'square' to move to, given 'pieces_col',
// which is the position of all pieces of the same colour as the knight.
uint64_t valid_knight_moves(uint64_t square, uint64_t pieces_col) {
	uint64_t allsquares = 0;
	allsquares |= square >> 10;
	allsquares |= square >> 6;
	allsquares |= square >> 15;
	allsquares |= square >> 17;
	allsquares |= square << 6;
	allsquares |= square << 10;
	allsquares |= square << 15;
	allsquares |= square << 17;
	return allsquares & ~pieces_col;
}