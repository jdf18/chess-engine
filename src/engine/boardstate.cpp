#include "boardstate.h"

#include <cstdint>
#include <unordered_map>

// Creates a hash table mapping every (individual) square to its rank
std::unordered_map<uint64_t, uint64_t> get_rank_mask() {
	std::unordered_map<uint64_t, uint64_t> mask = {};
	mask.rehash(64);
	uint64_t square = 1;
	for (int i = 0; i < 64; i++) {
		if ((square & FIRST_RANK) != 0) {
			mask.insert({ square, FIRST_RANK });
		}
		else if ((square & SECOND_RANK) != 0) {
			mask.insert({ square, SECOND_RANK });
		}
		else if ((square & THIRD_RANK) != 0) {
			mask.insert({ square, THIRD_RANK });
		}
		else if ((square & FOURTH_RANK) != 0) {
			mask.insert({ square, FOURTH_RANK });
		}
		else if ((square & FIFTH_RANK) != 0) {
			mask.insert({ square, FIFTH_RANK });
		}
		else if ((square & SIXTH_RANK) != 0) {
			mask.insert({ square, SIXTH_RANK });
		}
		else if ((square & SEVENTH_RANK) != 0) {
			mask.insert({ square, SEVENTH_RANK });
		}
		else if ((square & EIGHTH_RANK) != 0) {
			mask.insert({ square, EIGHTH_RANK });
		}
	}
	return mask;
}

// Creates a hash table mapping every (individual) square to its file
std::unordered_map<uint64_t, uint64_t> get_file_mask() {
	std::unordered_map<uint64_t, uint64_t> mask = {};
	mask.rehash(64);
	uint64_t square = 1;
	for (int i = 0; i < 64; i++) {
		if ((square & FIRST_FILE) != 0) {
			mask.insert({ square, FIRST_FILE });
		}
		else if ((square & SECOND_FILE) != 0) {
			mask.insert({ square, SECOND_FILE });
		}
		else if ((square & THIRD_FILE) != 0) {
			mask.insert({ square, THIRD_FILE });
		}
		else if ((square & FOURTH_FILE) != 0) {
			mask.insert({ square, FOURTH_FILE });
		}
		else if ((square & FIFTH_FILE) != 0) {
			mask.insert({ square, FIFTH_FILE });
		}
		else if ((square & SIXTH_FILE) != 0) {
			mask.insert({ square, SIXTH_FILE });
		}
		else if ((square & SEVENTH_FILE) != 0) {
			mask.insert({ square, SEVENTH_FILE });
		}
		else if ((square & EIGHTH_FILE) != 0) {
			mask.insert({ square, EIGHTH_FILE });
		}
	}
	return mask;
}

// Creates a hash table mapping every (individual) square to its north-east diagonal.
std::unordered_map<uint64_t, uint64_t> get_diag_mask_ne() {
	std::unordered_map<uint64_t, uint64_t> mask = {};
	mask.rehash(64);
	uint64_t square = 1;
	for (int i = 0; i < 64; i++) {
		uint64_t diag = square;
		uint64_t copy = square;
		while (copy != 0) {
			copy >>= BOARD_ROW + BOARD_COL;
			diag |= copy;
		}
		copy = square;
		while (copy != 0) {
			copy <<= BOARD_ROW + BOARD_COL;
			diag |= copy;
		}
		// Fix wrap-around
		//diag &= (square & BOARD_WHITE_SQUARE_MASK) |= 0 ? BOARD_WHITE_SQUARE_MASK : BOARD_BLACK_SQUARE_MASK;
		//TODO: Fix this
		mask.insert({ square, diag });
	}
}

// Creates a hash table mapping every (individual) square to its north-west diagonal.
std::unordered_map<uint64_t, uint64_t> get_diag_mask_nw() {
	std::unordered_map<uint64_t, uint64_t> mask = {};
	mask.rehash(64);
	uint64_t square = 1;
	for (int i = 0; i < 64; i++) {
		uint64_t diag = square;
		uint64_t copy = square;
		while (copy != 0) {
			copy >>= BOARD_ROW - BOARD_COL;
			diag |= copy;
		}
		copy = square;
		while (copy != 0) {
			copy <<= BOARD_ROW - BOARD_COL;
			diag |= copy;
		}
		// Fix wrap-around
		diag &= (square & BOARD_WHITE_SQUARE_MASK) != 0 ? BOARD_WHITE_SQUARE_MASK : BOARD_BLACK_SQUARE_MASK;
		mask.insert({ square, diag });
	}
}

// This function is unholy.
std::unordered_map<uint16_t, uint8_t> get_rank_attacks() {
	// Define the map and allocate required space
	std::unordered_map<uint16_t, uint8_t> map = {};
	map.rehash(8 * 256);
	// This will go through all the squares on the rank
	uint8_t square = 1;
	while (square != 0) {
		for (int i = 0; i < 256; i++) {
			uint8_t occupancy = i;
			// goLeft will move to the left of our square, until it meets an occupied square.
			uint8_t goLeft = square >> 1;
			uint8_t attacks = 0;
			while ((goLeft != 0) && ((goLeft & occupancy) == 0)) {
				attacks |= goLeft;
				goLeft >>= 1;
			}
			
			// Add the square that was occupied (This is the intended behaviour of the function).
			attacks |= goLeft;

			// goRight will move to the right of our square, until it meets an occupied square.
			uint8_t goRight = square << 1;
			while ((goRight != 0) && ((goRight & occupancy) == 0)) {
				attacks |= goRight;
				goRight <<= 1;
			}

			// Add the square that was occupied (This is the intended behaviour of the function).
			attacks |= goRight;

			// Create the key that will be used to look up (This is subject to change).
			uint16_t key = 0;
			*(uint8_t*)&key = square;
			*(uint8_t*)(&key + 1) = occupancy;
			map.insert({ key, attacks });
		}
	}
	return map;
}

inline constexpr BitBoard translate(BitBoard pieces, int8_t row_mod, int8_t col_mod) {
    // row_mod is the number of rows down, col_mod is the number of colums left
    // Work out which columns would be in if wrapping was allowed
	// Shift all the bits by the required amount
	// Apply the mask to remove the illegal moves

	BitBoard columns_mask = 0;
	if (col_mod < 0) {
		columns_mask = FIRST_FILE;
		for (int i = 1; i < -col_mod; i++) {
			columns_mask |= columns_mask >> 1;
		}
	} else if (col_mod > 0) {
		columns_mask = EIGHTH_FILE;
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

	if ((square & FIRST_FILE) > 0) {
		allsquares &= ~EIGHTH_FILE;
	}
	else if ((square & EIGHTH_FILE) > 0) {
		allsquares &= ~FIRST_FILE;
	}

	return allsquares;

}
