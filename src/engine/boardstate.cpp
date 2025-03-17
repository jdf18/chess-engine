#include "boardstate.h"

#include <cstdint>

inline constexpr BitBoard translate(BitBoard pieces, int8_t row_mod, int8_t col_mod) {
    // row_mod is the number of rows down, col_mod is the number of colums left
    // Work out which columns would be in if wrapping was allowed
	// Shift all the bits by the required amount
	// Apply the mask to remove the illegal moves

	BitBoard columns_mask = 0;
	if (col_mod < 0) {
		columns_mask = BOARD_LEFT_COL_MASK;
		for (int i = 1; i < -col_mod; i++) {
			columns_mask |= columns_mask >> 1;
		}
	} else if (col_mod > 0) {
		columns_mask = BOARD_RIGHT_COL_MASK;
		for (int i = 1; i < col_mod; i++) {
			columns_mask |= columns_mask << 1;
		}
	}

	if (row_mod > 0) {
		return (pieces >> (BOARD_ROW * row_mod + BOARD_COL * col_mod)) & (~columns_mask);
	} else if (row_mod < 0) {
		return (pieces << (BOARD_ROW * row_mod - BOARD_COL * col_mod)) & (~columns_mask);
	}
	if (col_mod >= 0) {
		return (pieces >> col_mod) & columns_mask;
	}
	return (pieces << -col_mod) & columns_mask;
}


// Returns all the valid squares for a knight on 'square' to move to, given 'friendly_pieces',
// which is the position of all pieces of the same colour as the knight.
BitBoard BoardState::pseudo_legal_knights_moves(Colour colour) {
    BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
    BitBoard friendly_knights = pieces_knights & friendly_pieces;

	BitBoard possible_moves = 0;
    // Set bits of squares which the knight can move to

	// Get a bitboard of the possible moves of all knights on the board
	possible_moves |= translate(friendly_knights,  1,  2);
	possible_moves |= translate(friendly_knights,  1, -2);
	possible_moves |= translate(friendly_knights, -1,  2);
	possible_moves |= translate(friendly_knights, -1, -2);
	possible_moves |= translate(friendly_knights,  2,  1);
	possible_moves |= translate(friendly_knights,  2, -1);
	possible_moves |= translate(friendly_knights, -2,  1);
	possible_moves |= translate(friendly_knights, -2, -1);

    // Can not move to squares that are occupied by own pieces
    possible_moves &= ~friendly_pieces;

	return possible_moves;
}

// Returns all the valid movs for a pawn on 'square' to move to, given 'pieces',
// which is the position of all pieces, and 'friendly_pieces', the position of
// all pieces of the same colour as the pawn.
// This does factor in the pawn making a double move if it's on the second rank.
// It requires a boolean for the colour. True for white and False for black.
BitBoard BoardState::pseudo_legal_pawn_moves(Colour colour) {
	// In the case that the pawn is white, it moves two squares if on the second rank.
	// The direction it moves is also different.
	// I don't think this can be written without the branch on colour, but the second 
	//   branch (checking if it's on the second rank) could maybe
	// be removed with some bit manipulation wizardry.
	
	// Ive moved this function to now be a method as part of the BoardState class. 
	//   This setup is to get all information that was previously passed into the function, out of the struct.
	// TODO: This function will probably be optimised/cleaned up in the future
	// Old function definition is below:
	//   uint64_t pseudo_legal_pawn_moves(uint64_t square, uint64_t pieces, uint64_t friendly_pieces, uint64_t enemy_pieces, bool white)

	BitBoard square = pieces_pawns;
	BitBoard pieces = pieces_white | pieces_black;

	BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
	BitBoard enemy_pieces = (colour != COL_WHITE ? pieces_white : pieces_black);

	bool white = (colour == COL_WHITE);

	uint64_t allsquares = 0;

	if (white) {
		// Check if the pawn is on the second rank
		if ((square & (0xFF << BOARD_ROW)) > 0) {
			// Add the two squares the pawn could move to in theory
			allsquares |= square << BOARD_ROW;
			allsquares |= square << (2 * BOARD_ROW);
			// Remove squares occupied by pieces
			allsquares &= ~pieces;
			// This is potentially incorrect, but I think it should work.
			// We XOR the spaces occupied by other pieces with the space the pawn is on, so it doesn't interfere.
			// We then take these spaces and move them all up one square (by left shifting by 8).
			// We can then bitwise AND allsquares with NOT of this.
			// This should take care of the case where there is a piece directly in front of the pawn, but not the square after that.
			allsquares &= ~((pieces ^ square) << BOARD_ROW);
		}
		else {
			allsquares |= square << BOARD_ROW;
			allsquares &= ~pieces;
		}
		allsquares |= ((square << (BOARD_ROW - BOARD_COL)) | (square << (BOARD_ROW + BOARD_COL))) & enemy_pieces;
		allsquares &= ~friendly_pieces;
	}
	else {
		// Check if the pawn is on the seventh rank
		if ((square & (0xFF << (6 * BOARD_ROW))) > 0) {
			// Add the two squares the pawn could move to in theory
			allsquares |= square >> BOARD_ROW;
			allsquares |= square >> (2 * BOARD_ROW);
			// Remove squares occupied by pieces
			allsquares &= ~pieces;
			// This should take care of the case where there is a piece directly in front of the pawn, but not the square after that.
			// Same reasoning as for the white case.
			allsquares &= ~((pieces ^ square) >> BOARD_ROW);
		}
		else {
			allsquares |= square >> BOARD_ROW;
			allsquares &= ~pieces;
		}
		allsquares |= ((square >> (BOARD_ROW - BOARD_COL)) | (square >> (BOARD_ROW + BOARD_COL))) & enemy_pieces;
		allsquares &= ~friendly_pieces;
		
	}

	if ((square & BOARD_LEFT_COL_MASK) > 0) {
		allsquares &= ~BOARD_RIGHT_COL_MASK;
	}
	else if ((square & BOARD_RIGHT_COL_MASK) > 0) {
		allsquares &= ~BOARD_LEFT_COL_MASK;
	}

	return allsquares;

}