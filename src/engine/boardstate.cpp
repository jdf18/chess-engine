#include "boardstate.h"

#include <cstdint>
#include <unordered_map>


std::unordered_map<uint64_t, uint64_t> file_masks = get_file_mask();
std::unordered_map<uint64_t, uint64_t> rank_masks = get_rank_mask();
std::unordered_map<uint64_t, uint64_t> diag_masks_ne = get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> diag_masks_nw = get_diag_mask_nw();
std::unordered_map<uint16_t, uint8_t> rank_attacks_map = get_rank_attacks();
std::unordered_map<std::bitset<128>, uint64_t> diag_attacks_ne;
std::unordered_map<std::bitset<128>, uint64_t> diag_attacks_nw;

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
		square <<= 1;
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
		square <<= 1;
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
		diag &= ((square & BOARD_WHITE_SQUARE_MASK) != 0) ? BOARD_WHITE_SQUARE_MASK : BOARD_BLACK_SQUARE_MASK;
		mask.insert({ square, diag });
		square <<= 1;
	}
	return mask;
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
		diag &= ((square & BOARD_WHITE_SQUARE_MASK) != 0) ? BOARD_WHITE_SQUARE_MASK : BOARD_BLACK_SQUARE_MASK;
		mask.insert({ square, diag });
		square <<= 1;
	}
	return mask;
}

// This function is unholy.
// Creates a hash table mapping the square (on a rank) that a piece is on and the rank occupancy to all squares on that rank attacked by that piece.
// This same hash table can be used for files if we do some processing to the file first.
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
			uint16_t key = (square << 8) | occupancy;
			map.insert({ key, attacks });
		}
		square++;
	}
	return map;
}

std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_ne() {

	// Start with the piece on the first square.
	uint64_t piece_pos = 1;
	
	// Keep track of the index of the square the piece is on (this is useful later)
	int piece_idx = 0;

	//Create the hash table we will use.
	std::unordered_map<std::bitset<128>, uint64_t> output = {};

	// Loop once for each piece position
	while (piece_pos > 0) {
		//Get the diagonal that the piece is on, and count the number of squares
		uint64_t diagonal = diag_masks_ne[piece_pos];
		int diag_length = std::bitset<64>(diagonal).count();

		//Get the position of the first square in the diagonal (bottom-left square on the board on this diagonal), store it in first_pos
		int first_pos = 0;
		uint64_t diag_copy = diagonal;
		while ((diag_copy & 1) == 0) {
			diag_copy >>= 1;
			first_pos++;
		}

		//Loop through every possible board occupancy for the diagonal that the square is on
		for (int i = 0; i < pow(2, diag_length); i++) {
			uint64_t occupancy = 0;
			//j is a bit which masks the bits we want from i, so we can spread i along the diagonal.
			uint8_t j = 1;
			int shift_num = 0;
			//Loop until we have gone through every square in the diagonal
			while (shift_num < diag_length) {
				//mask the bit we want from i (the occupancy), shift it to the least significant spot, then left shift it to where we want.
				occupancy |= ((uint64_t)((i & j) >> shift_num)) << (first_pos + (shift_num * (BOARD_ROW + BOARD_COL)));
				
				//Then j is shifted by 1 to be the next bit, and shift_num is incremented so we go to the next square on the diagonal.
				j <<= 1;
				shift_num += 1;
			}
			// Create the key
			std::bitset<128> key;
			// Set the first 64 bits to the square of the piece
			key.set(piece_idx + 64);
			// Set the next 64 bits to the occupancy we're working with
			for (int k = 0; k < 64; k++) {
				key.set(k, (occupancy >> k) & 1);
			}
			
			int left_squares = (piece_idx - first_pos) / 9; // Get the number of squares to the left/right of the piece on the diagonal
			int right_squares = diag_length - left_squares - 1; // take 1 since we don't count the square the piece is on

			//Create a copy of the piece_pos bit, which will travel left and right, checking against the occupancy.
			uint64_t pos_copy = piece_pos >> 9;

			//Create our bitboard, which will represent the squares attacked by the piece on this diagonal.
			uint64_t attacks = 0;

			//Repeat once for each square to the left (down-left diagonally) of the piece
			for (int k = 0; k < left_squares; k++) {
				//Add the pos-copy to the attacks. We want to include ther square of the piece we 'hit' in our attacks vector,
				//so we can add it before we check if the square is occupied
				attacks |= pos_copy;

				//break if the square we just added is occupied
				if ((pos_copy & occupancy) != 0) {
					break;
				}

				//If not occupied, we go to the next square and repeat.
				pos_copy >>= 9;
			}

			//The logic for this is the same as for the left, but just travelling in the opposite direction.
			pos_copy = piece_pos << 9;
			for (int k = 0; k < right_squares; k++) {
				attacks |= pos_copy;
				if ((pos_copy & occupancy) != 0) {
					break;
				}
				pos_copy >>= 9;
			}

			//Insert the key and attacks.
			output.insert({ key, attacks });
		}
	}
}

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
		output |= rank << i;
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

BitBoard BoardState::pseudo_legal_rook_moves(Colour colour, BitBoard square) {
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