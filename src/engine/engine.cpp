#include <cstdint>

#define ROW 8
#define COL 1
#define LEFT_COL_MASK 0x0101010101010101
#define RIGHT_COL_MASK 0x8080808080808080
#define WHITE_SQUARE_MASK 0x55AA55AA55AA55AA
#define BLACK_SQUARE_MASK ~WHITE_SQUARE_MASK

// bitboards: 64 bit unsigned integers, with 1 bit for each square of the board.
// There will be quite a few different bitboards, containing useful information.
// The least significant bit is the square a1. The next least significant is b1, so on and so forth. The ninth least significant will be a2.
// And the most significant is h8.
// This means that a left shift of 1 corresponds to moving right one square, and a right shift of 1 corresponds to moving left one square.
// A left shift of 8 corresponds to moving up one square, and a right shift of 8 corresponds to moving down one square.
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
	allsquares |= square >> (ROW + 2 * COL);
	allsquares |= square >> (ROW - 2 * COL);
	allsquares |= square >> (2 * ROW - COL);
	allsquares |= square >> (2 * ROW + COL);
	allsquares |= square << (ROW - 2 * COL);
	allsquares |= square << (ROW + 2 * COL);
	allsquares |= square << (2 * ROW - COL);
	allsquares |= square << (2 * ROW + COL);
	return allsquares & ~pieces_col & (((square & WHITE_SQUARE_MASK) > 0) ? BLACK_SQUARE_MASK : WHITE_SQUARE_MASK);
}

// Returns all the valid movs for a pawn on 'square' to move to, given 'pieces',
// which is the position of all pieces, and 'pieces_col', the position of
// all pieces of the same colour as the pawn.
// This does factor in the pawn making a double move if it's on the second rank.
// It requires a boolean for the colour. True for white and False for black.
uint64_t valid_pawn_moves(uint64_t square, uint64_t pieces, uint64_t pieces_col, uint64_t pieces_opposite_col, bool white) {
	// In the case that the pawn is white, it moves two squares if on the second rank.
	// The direction it moves is also different.
	// I don't think this can be written without the branch on colour, but the second branch (checking if it's on the second rank) could maybe
	// be removed with some bit manipulation wizardry.
	uint64_t allsquares = 0;

	if (white) {
		// Check if the pawn is on the second rank
		if ((square & (0xFF << ROW)) > 0) {
			// Add the two squares the pawn could move to in theory
			allsquares |= square << ROW;
			allsquares |= square << (2 * ROW);
			// Remove squares occupied by pieces
			allsquares &= ~pieces;
			// This is potentially incorrect, but I think it should work.
			// We XOR the spaces occupied by other pieces with the space the pawn is on, so it doesn't interfere.
			// We then take these spaces and move them all up one square (by left shifting by 8).
			// We can then bitwise AND allsquares with NOT of this.
			// This should take care of the case where there is a piece directly in front of the pawn, but not the square after that.
			allsquares &= ~((pieces ^ square) << ROW);
		}
		else {
			allsquares |= square << ROW;
			allsquares &= ~pieces;
		}
		allsquares |= ((square << (ROW - COL)) | (square << (ROW + COL))) & pieces_opposite_col;
		allsquares &= ~pieces_col;
	}
	else {
		// Check if the pawn is on the seventh rank
		if ((square & (0xFF << (6 * ROW))) > 0) {
			// Add the two squares the pawn could move to in theory
			allsquares |= square >> ROW;
			allsquares |= square >> (2 * ROW);
			// Remove squares occupied by pieces
			allsquares &= ~pieces;
			// This should take care of the case where there is a piece directly in front of the pawn, but not the square after that.
			// Same reasoning as for the white case.
			allsquares &= ~((pieces ^ square) >> ROW);
		}
		else {
			allsquares |= square >> ROW;
			allsquares &= ~pieces;
		}
		allsquares |= ((square >> (ROW - COL)) | (square >> (ROW + COL))) & pieces_opposite_col;
		allsquares &= ~pieces_col;
		
	}

	if ((square & LEFT_COL_MASK) > 0) {
		allsquares &= ~RIGHT_COL_MASK;
	}
	else if ((square & RIGHT_COL_MASK) > 0) {
		allsquares &= ~LEFT_COL_MASK;
	}

	return allsquares;

}