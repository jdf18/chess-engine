#include "boardstate.h"

//uint16_t BoardState::get_file_key(uint64_t piece_square) {
//	//Get the occupancy for the whole board
//	uint64_t board_occupancy = pieces_white.board | pieces_black.board;
//	//Get the file mask that the square is on
//	uint64_t file_mask = file_masks[piece_square];
//	//Mask only the occupancy for the file the square is on.
//	uint64_t file_occupancy = board_occupancy & file_mask;
//	//Shift the file to the left (by doing right shifts) until it is on the first file
//	//Keep track of the required shifts so that we can do it again for the square
//	int shift_counter = 0;
//	//This condition will always be satisfied eventually as the square the piece is on is part of the file.
//	while ((file_occupancy & FIRST_FILE) == 0) {
//		shift_counter += 1;
//		file_occupancy >>= 1;
//	}
//	//Shift the bits so that they make up an 8-bit section of our result
//	uint64_t compressed_file = file_occupancy;
//	for (int i = 7; i <= 49; i += 7) {
//		compressed_file |= file_occupancy >> i;
//	}
//	//Mask out the bits we want
//	compressed_file &= 0xFF;
//
//	//Get the piece on the right square
//	uint64_t piece_pos = piece_square >> shift_counter;
//
//	//Keep shifting in multiples of 7
//	for (int i = 7; i <= 49; i += 7) {
//		piece_pos |= piece_pos >> i;
//	}
//	//Again mask the bits we want
//	piece_pos &= 0xFF;
//	//Return a valid key
//	return (piece_pos << 8) | compressed_file;
//}

inline uint64_t rank_to_file(uint8_t rank) {
	uint64_t output = 0;
	for (int i = 0; i <= 49; i += 7) {
		output |= (uint64_t)rank << i;
	}
	output &= FIRST_FILE;
	return output;
}

inline uint8_t file_to_rank(uint64_t file) {
	uint64_t output = file;
	for (int i = 7; i <= 49; i += 7) {
		output |= file >> i;
	}
	output &= FIRST_RANK;
	return output;
}

inline BitBoard translate(BitBoard pieces, int8_t row_mod, int8_t col_mod) {
    // row_mod is the number of rows down, col_mod is the number of colums left
    // Work out which columns would be in if wrapping was allowed
	// Shift all the bits by the required amount
	// Apply the mask to remove the illegal moves

	BitBoard columns_mask = 0;
	if (col_mod < 0) {
		columns_mask = FIRST_FILE;
		for (int i = 1; i < -col_mod; i++) {
			columns_mask |= columns_mask << 1;
		}
	} else if (col_mod > 0) {
		columns_mask = EIGHTH_FILE;
		for (int i = 1; i < col_mod; i++) {
			columns_mask |= columns_mask >> 1;
		}
	}

	if (row_mod > 0) {
		return (pieces >> (BOARD_ROW * row_mod + BOARD_COL * col_mod)) & (~columns_mask);
	} else if (row_mod < 0) {
		return (pieces << (BOARD_ROW * (-row_mod) - BOARD_COL * col_mod)) & (~columns_mask);
	}
	if (col_mod >= 0) {
		return (pieces >> col_mod) & (~columns_mask);
	}
	return (pieces << -col_mod) & (~columns_mask);
}


// Returns all the valid squares for a knight on 'square' to move to
BitBoard BoardState::pseudo_legal_knight_moves(BitBoard square) const {
	Colour colour = (square & pieces_white) != 0 ? COL_WHITE : COL_BLACK;
    BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);

	BitBoard possible_moves = 0;
    // Set bits of squares which the knight can move to

	// Get a bitboard of the possible moves of all knights on the board
	possible_moves |= translate(square,  1,  2);
	possible_moves |= translate(square,  1, -2);
	possible_moves |= translate(square, -1,  2);
	possible_moves |= translate(square, -1, -2);
	possible_moves |= translate(square,  2,  1);
	possible_moves |= translate(square,  2, -1);
	possible_moves |= translate(square, -2,  1);
	possible_moves |= translate(square, -2, -1);

    // Can not move to squares that are occupied by own pieces
    possible_moves &= ~friendly_pieces;

    return possible_moves;
}

// Returns all the valid movs for a pawn on 'square' to move to, given 'pieces',
// which is the position of all pieces, and 'friendly_pieces', the position of
// all pieces of the same colour as the pawn.
// This does factor in the pawn making a double move if it's on the second rank.
// It requires a boolean for the colour. True for white and False for black.
BitBoard BoardState::pseudo_legal_pawn_moves(BitBoard square) const {
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

	BitBoard pieces = pieces_white | pieces_black;
	Colour colour = (square & pieces_white) != 0 ? COL_WHITE : COL_BLACK;

	BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
	BitBoard enemy_pieces = (colour != COL_WHITE ? pieces_white : pieces_black);

	bool white = (colour == COL_WHITE);

	BitBoard allsquares = 0;

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

	}

	if ((square & FIRST_FILE) > 0) {
		allsquares &= ~EIGHTH_FILE;
	}
	else if ((square & EIGHTH_FILE) > 0) {
		allsquares &= ~FIRST_FILE;
	}

	return allsquares;

}

BitBoard BoardState::pseudo_legal_rook_moves(BitBoard square) const {
	Colour colour = (square & pieces_white) != 0 ? COL_WHITE : COL_BLACK;
	uint64_t friendly_pieces = (colour == COL_WHITE ? pieces_white.board : pieces_black.board);
	uint64_t all_pieces = pieces_white.board | pieces_black.board;
	uint64_t rank_mask = rank_masks[square.board];
	uint64_t file_mask = file_masks[square.board];

	//Get rank attacks:
	uint64_t rank_only = all_pieces & rank_mask;
	int shift_num = 0;
	while ((rank_only & FIRST_RANK) == 0) {
		shift_num++;
		rank_only >>= BOARD_ROW;
	}
	uint16_t rank_key = ((square.board >> (shift_num * BOARD_ROW)) << 8) | rank_only;
	uint64_t rank_attacks = (uint64_t)rank_attacks_map[rank_key] << (shift_num * BOARD_ROW);

	//Get file attacks
	uint64_t file_only = all_pieces & file_mask;
	shift_num = 0;
	while((file_only & FIRST_FILE) == 0) {
		shift_num++;
		file_only >>= BOARD_COL;
	}
	uint16_t file_key = (file_to_rank(square.board >> shift_num) << 8) | file_to_rank(file_only);
	uint64_t file_attacks = rank_to_file(rank_attacks_map[file_key]) << shift_num;

	return (rank_attacks | file_attacks) & ~friendly_pieces;

}

BitBoard BoardState::pseudo_legal_bishop_moves(BitBoard square) const {
	Colour colour = (square & pieces_white) != 0 ? COL_WHITE : COL_BLACK;
	uint64_t friendly_pieces = colour == COL_WHITE ? pieces_white.board : pieces_black.board;
	uint64_t all_pieces = pieces_white.board | pieces_black.board;
	uint64_t diag_mask_ne = diag_masks_ne[square.board];
	uint64_t diag_mask_nw = diag_masks_nw[square.board];

	// Get ne attacks:
	uint64_t ne_diag = diag_mask_ne & all_pieces;
	uint16_t key = std::countr_zero(square.board) << 8;
	uint8_t occupancy = 0;
	uint8_t first_pos = 0;
	uint8_t diag_length = std::bitset<64>(diag_mask_ne).count();
	while ((diag_mask_ne & 1) == 0) {
		first_pos++;
		diag_mask_ne >>= 1;
	}
	for (int i = 0; i < diag_length; i++) {
		occupancy |= ((ne_diag >> (first_pos + (i * (BOARD_ROW + BOARD_COL)))) << i) & (1 << i);
	}
	key |= occupancy;
	uint64_t ne_attacks = diag_attacks_ne[key];


	// Get nw attacks:
	uint64_t nw_diag = diag_mask_nw & all_pieces;
	key = std::countr_zero(square.board) << 8;
	occupancy = 0;
	first_pos = 0;
	diag_length = std::bitset<64>(diag_mask_nw).count();
	while ((diag_mask_nw & 1) == 0) {
		first_pos++;
		diag_mask_nw >>= 1;
	}
	for (int i = 0; i < diag_length; i++) {
		occupancy |= ((nw_diag >> (first_pos + (i * (BOARD_ROW - BOARD_COL)))) << i) & (1 << i);
	}
	key |= occupancy;
	uint64_t nw_attacks = diag_attacks_nw[key];

	//Return the attacks
	return (ne_attacks | nw_attacks) & ~friendly_pieces;
}

BitBoard BoardState::pseudo_legal_queen_moves(BitBoard square) const {
	return (pseudo_legal_rook_moves(square) | pseudo_legal_bishop_moves(square));
}

BitBoard BoardState::pseudo_legal_king_moves(BitBoard square) const {
	Colour colour = (square & pieces_white) != 0 ? COL_WHITE : COL_BLACK;
	BitBoard out = 0;
	BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
	out |= translate(square, -1, -1);
	out |= translate(square, -1, 0);
	out |= translate(square, -1, 1);
	out |= translate(square, 0, 1);
	out |= translate(square, 1, 1);
	out |= translate(square, 1, 0);
	out |= translate(square, 1, -1);
	out |= translate(square, 0, -1);
	out &= ~friendly_pieces;
	return out;
}