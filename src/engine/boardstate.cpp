#include "boardstate.h"

#include <cstdint>


// Returns all the valid squares for a knight on 'square' to move to, given 'friendly_pieces',
// which is the position of all pieces of the same colour as the knight.
BitBoard BoardState::pseudo_legal_knights_moves(Colour colour) {
    BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
    BitBoard friendly_knights = pieces_knights & friendly_pieces;

	uint64_t possible_moves = 0;
    // Set bits of squares which the knight can move to
	possible_moves |= friendly_knights >> (BOARD_ROW + 2 * BOARD_COL);
	possible_moves |= friendly_knights >> (BOARD_ROW - 2 * BOARD_COL);
	possible_moves |= friendly_knights >> (2 * BOARD_ROW - BOARD_COL);
	possible_moves |= friendly_knights >> (2 * BOARD_ROW + BOARD_COL);
	possible_moves |= friendly_knights << (BOARD_ROW - 2 * BOARD_COL);
	possible_moves |= friendly_knights << (BOARD_ROW + 2 * BOARD_COL);
	possible_moves |= friendly_knights << (2 * BOARD_ROW - BOARD_COL);
	possible_moves |= friendly_knights << (2 * BOARD_ROW + BOARD_COL);
    // TODO: Check to see if goes out of bounds of the board

    // Can not move to squares that are occupied by own pieces
    possible_moves &= ~friendly_pieces;
    
    // Dont think we really need the code below anymore, also works if only a single bit 
	//   is set in pieces_knights, so would have to iterate through each one.
	//   Commented out for now incase we need to add it back later.
    
	// Check that the only possible squares it can move to are ones of 
	//   a colour it was not previously on
    // possible_moves &= (((friendly_knights & BOARD_WHITE_SQUARE_MASK) > 0) ? BOARD_BLACK_SQUARE_MASK : BOARD_WHITE_SQUARE_MASK)

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
		if ((square & SECOND_RANK) > 0) {
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
		if ((square & SEVENTH_RANK) > 0) {
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

	if ((square & FIRST_COL) > 0) {
		allsquares &= ~EIGHTH_COL;
	}
	else if ((square & EIGHTH_COL) > 0) {
		allsquares &= ~FIRST_COL;
	}

	return allsquares;

}