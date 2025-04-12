#include "boardstate.h"

#include <cstdint>
#include <unordered_map>
#include <cmath>


std::unordered_map<uint64_t, uint64_t> file_masks = get_file_mask();
std::unordered_map<uint64_t, uint64_t> rank_masks = get_rank_mask();
std::unordered_map<uint64_t, uint64_t> diag_masks_ne = get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> diag_masks_nw = get_diag_mask_nw();
std::unordered_map<uint16_t, uint8_t> rank_attacks_map = get_rank_attacks();
std::unordered_map<uint16_t, uint64_t> diag_attacks_ne = get_diag_attacks_ne_better();
std::unordered_map<uint16_t, uint64_t> diag_attacks_nw = get_diag_attacks_nw_better();
std::unordered_map<uint64_t, int> square_to_index_map = get_square_to_index_map();


std::unordered_map<uint64_t, int> get_square_to_index_map() {
	std::unordered_map<uint64_t, int> map = {};
	map.rehash(64);
	uint64_t key = 1;
	int index = 0;
	while (key > 0) {
		map.insert({ key, index });
		key <<= 1;
		index++;
	}
	return map;
}

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
	//This is the number of entries. Calculated as 8 * 256 + 2 * 7 * 128 + 2 * 6 * 64 + 2 * 5 * 32 + 2 * 4 * 16 + 2 * 3 * 8 + 2 * 2 * 4 + 2 * 1 * 2.
	output.rehash(5124);

	// Loop once for each piece position
	while (piece_pos > 0) {
		//Get the diagonal that the piece is on, and count the number of squares
		uint64_t diagonal = diag_masks_ne[piece_pos];
		int diag_length = std::bitset<64>(diagonal).count();

		//Get the position of the first square in the diagonal (bottom-left square on the board on this diagonal), store it in first_pos
		int first_pos = 0;
		uint64_t diag_copy = diagonal;
		while ((diag_copy & 1) == 0 && diag_copy != 0) {
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
		piece_pos >>= 1;
		piece_idx++;
	}
	return output;
}

std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_nw() {
	uint64_t piece_pos = 1;
	int piece_idx = 0;
	std::unordered_map<std::bitset<128>, uint64_t> output = {};
	output.rehash(5124);

	//Loop once for each piece position
	while (piece_pos > 0) {
		uint64_t diagonal = diag_masks_nw[piece_pos];
		int diag_length = std::bitset<64>(diagonal).count();

		int first_pos = 0;
		uint64_t diag_copy = diagonal;
		while ((diag_copy & 1) == 0 && diag_copy != 0) {
			first_pos++;
			diag_copy >>= 1;
		}
		for (int i = 0; i < pow(2, diag_length); i++) {
			uint64_t occupancy = 0;
			uint8_t j = 1;
			int shift_num = 0;
			while (shift_num < diag_length) {
				occupancy |= ((uint64_t)((i & j) >> shift_num)) << (first_pos + (shift_num * (BOARD_ROW - BOARD_COL)));
				j <<= 1;
				shift_num += 1;
			}

			std::bitset<128> key;
			key.set(piece_idx + 64);
			for (int k = 0; k < 64; k++) {
				key.set(k, (occupancy >> k) & 1);
			}

			int right_squares = (piece_idx - first_pos) / 7;
			int left_squares = diag_length - right_squares - 1;

			uint64_t pos_copy = piece_pos >> 7;

			uint64_t attacks = 0;

			for (int k = 0; k < right_squares; k++) {
				attacks |= pos_copy;

				if ((pos_copy & occupancy) != 0) {
					break;
				}
				pos_copy >>= 7;
			}

			pos_copy = piece_pos << 7;
			for (int k = 0; k < left_squares; k++) {
				attacks |= pos_copy;
				if ((pos_copy & occupancy) != 0) {
					break;
				}
				pos_copy >>= 7;
			}

			//Insert the key and attacks.
			output.insert({ key, attacks });
		}
		piece_pos <<= 1;
		piece_idx++;
	}
	return output;
}

std::unordered_map<uint16_t, uint64_t> get_diag_attacks_ne_better() {
	//setup the piece index and piece pos.
	//the piece index is the index of the piece within the bitboard, and piece pos is the actual bitboard for the piece.
	uint8_t piece_index = 0;
	uint64_t piece_pos = 1;

	//Setup the hash table
	std::unordered_map<uint16_t, uint64_t> map = {};

	map.rehash(5124);

	//Loop once for each piece position
	while (piece_pos > 0) {
		//Get the diagonal that the piece is on, and count the number of squares.
		uint64_t diag = diag_masks_ne[piece_pos];
		uint8_t diag_length = std::bitset<64>(diag).count();

		//Get the index of the first square in the diagonal
		uint8_t first_index = 0;
		uint64_t diag_copy = diag;
		while ((diag_copy & 1) == 0 && diag_copy != 0) {
			first_index++;
			diag_copy >>= 1;
		}

		//Repeat once for each possible occupancy
		for (uint16_t occupancy = 0; occupancy < pow(2, diag_length); occupancy++) {
			//Get the number of squares to the left of our piece, and the number of squares to its right.
			uint8_t left_squares = (piece_index - first_index) / 9;
			uint8_t right_squares = diag_length - left_squares - 1;
			
			//Setup j, which will be a generic variable, a single bit shifted left and right for masking and such.
			//j starts one space to the left of our piece, within the 8-bit occupancy space.
			uint8_t j;
			if (left_squares > 0) {
				j = 1 << (left_squares - 1);
			} 
			else {
				j = 0;
			}
			//Get the attacks
			uint8_t attacks = 0;

			//Repeat once for each square to the left of our piece.
			for (int i = 0; i < left_squares; i++) {

				//Add the bit represented by j, then break if there is a piece at that location.
				attacks |= j;
				if ((j & occupancy) != 0) {
					break;
				}

				//Move left one (by right shifting)
				j >>= 1;
			}

			//Set j up to be one square to the right of our piece.
			if (left_squares < 7) {
				j = 1 << (left_squares + 1);
			}
			else {
				j = 0;
			}

			//Repeat once for each square to the right of our piece.
			for (int i = 0; i < right_squares; i++) {

				//Add the bit represented by j, then break if there is a piece at that location.
				attacks |= j;
				if ((j & occupancy) != 0) {
					break;
				}

				//Move right one (by left shifting)
				j <<= 1;
			}

			//Create the key for our entry. The first 8 bits is just the index of the piece.
			uint16_t key = (uint16_t)(piece_index) << 8;
			//The second 8 bits is the 8-bit (or smaller) occupancy value.
			key |= occupancy;

			//Now create the value for our entry.
			uint64_t output = 0;
			uint8_t shift_num = 0;

			//j is used again, this time to go from least significant to most significant bit of our attacks, spacing them out correctly over the diagonal.
			j = 1;

			//Repeat once for each square on the diagonal
			while (shift_num < diag_length) {
				//Get the correct bit of the attacks, shift so it's the least significant bit, then left shift it to the right place on the diagonal.
				output |= (uint64_t)((attacks & j) >> shift_num) << (first_index + (shift_num * (BOARD_ROW + BOARD_COL)));

				//Now increment shift num, and left shift j by 1.
				shift_num++;
				j <<= 1;
			}

			//Insert the key and value into the table.
			map.insert({ key, output });
		}

		//Left shift the piece by 1, moving to the next square, and increment the piece index.
		piece_pos <<= 1;
		piece_index++;
	}
	return map;
}

std::unordered_map<uint16_t, uint64_t> get_diag_attacks_nw_better() {
	uint8_t piece_index = 0;
	uint64_t piece_pos = 1;

	std::unordered_map<uint16_t, uint64_t> map = {};

	map.rehash(5124);

	while (piece_pos > 0) {
		uint64_t diag = diag_masks_nw[piece_pos];
		uint8_t diag_length = std::bitset<64>(diag).count();
		uint8_t first_index = 0;
		uint64_t diag_copy = diag;
		while ((diag_copy & 1) == 0 && diag_copy != 0) {
			first_index++;
			diag_copy >>= 1;
		}
		for (uint16_t occupancy = 0; occupancy < pow(2, diag_length); occupancy++) {
			uint8_t right_squares = (piece_index - first_index) / 7;
			uint8_t left_squares = diag_length - right_squares - 1;
			uint8_t j;
			if (right_squares > 0) {
				j = 1 << (right_squares - 1);
			}
			else {
				j = 0;
			}
			uint8_t attacks = 0; 
			for (int i = 0; i < right_squares; i++) {
				attacks |= j;
				if ((j & occupancy) != 0) {
					break;
				}
				j >>= 1;
			}
			if (right_squares < 7) {
				j = 1 << (right_squares + 1);
			}
			else {
				j = 0;
			}
			for (int i = 0; i < left_squares; i++) {
				attacks |= j;
				if ((j & occupancy) != 0) {
					break;
				}
				j <<= 1;
			}
			uint16_t key = (uint16_t)(piece_index) << 8;
			key |= occupancy;
			uint64_t output = 0;
			uint8_t shift_num = 0;
			j = 1;
			while (shift_num < diag_length) {
				output |= (uint64_t)((attacks & j) >> shift_num) << (first_index + (shift_num * (BOARD_ROW - BOARD_COL)));
				shift_num++;
				j <<= 1;
			}
			map.insert({ key, output });
		}

		piece_pos <<= 1;
		piece_index++;
	}
	return map;
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
BitBoard BoardState::pseudo_legal_knight_moves(Colour colour) {
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

BitBoard BoardState::pseudo_legal_bishop_moves(Colour colour, BitBoard square) {
	uint64_t friendly_pieces = (colour == COL_WHITE ? pieces_white.board : pieces_black.board);
	uint64_t all_pieces = pieces_white.board | pieces_black.board;
	uint64_t diag_mask_ne = diag_masks_ne[square.board];
	uint64_t diag_mask_nw = diag_masks_nw[square.board];

	// Get ne attacks:
	uint64_t ne_diag = diag_mask_ne & all_pieces;
	uint16_t key = square_to_index_map[square.board] << 8;
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
	key = square_to_index_map[square.board] << 8;
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

BitBoard BoardState::pseudo_legal_queen_moves(Colour colour, BitBoard square) {
	return (pseudo_legal_rook_moves(colour, square) | pseudo_legal_bishop_moves(colour, square));
}

BitBoard BoardState::pseudo_legal_king_moves(Colour colour) {
	BitBoard out = 0;
	BitBoard friendly_pieces = (colour == COL_WHITE ? pieces_white : pieces_black);
	BitBoard king = pieces_kings & friendly_pieces;
	out |= translate(king, -1, -1);
	out |= translate(king, -1, 0);
	out |= translate(king, -1, 1);
	out |= translate(king, 0, 1);
	out |= translate(king, 1, 1);
	out |= translate(king, 1, 0);
	out |= translate(king, 1, -1);
	out |= translate(king, 0, -1);
	out &= ~friendly_pieces;
	return out;
}

PieceInstance BoardState::get_piece(uint8_t row, uint8_t column) {
	const BitBoard square_mask = (static_cast<uint64_t>(0x1) << (row + (8 * column)));
	Colour piece_colour;

	if ((pieces_white & square_mask).board != 0) {
		piece_colour = COL_WHITE;
	} else if ((pieces_black & square_mask).board != 0) {
		piece_colour = COL_BLACK;
	} else {
		return PieceInstance{std::make_unique<Piece>(COL_NONE, PIECE_NONE), SquarePosition{row, column}};
	}

	if ((pieces_kings & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_KING), SquarePosition{row, column}};
	} else if ((pieces_queens & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_QUEEN), SquarePosition{row, column}};
	} else if ((pieces_rooks & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_ROOK), SquarePosition{row, column}};
	} else if ((pieces_bishops & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_BISHOP), SquarePosition{row, column}};
	} else if ((pieces_knights & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_KNIGHT), SquarePosition{row, column}};
	} else if ((pieces_pawns & square_mask).board != 0) {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_PAWN), SquarePosition{row, column}};
	} else {
		return PieceInstance{std::make_unique<Piece>(piece_colour, PIECE_NONE), SquarePosition{row, column}};
	}
}

void BoardState::print() {
	for (uint8_t column = 0; column < 8; column++) {
		for (uint8_t row = 0; row < 8; row++) {
			const PieceInstance piece_instance = get_piece(row, column);
			char piece_character = ' ';
			switch (piece_instance.piece->type) {
				case PIECE_KING:
					piece_character = 'k';
					break;
				case PIECE_QUEEN:
					piece_character = 'q';
					break;
				case PIECE_ROOK:
					piece_character = 'r';
					break;
				case PIECE_BISHOP:
					piece_character = 'b';
					break;
				case PIECE_KNIGHT:
					piece_character = 'n';
					break;
				case PIECE_PAWN:
					piece_character = 'p';
					break;
				case PIECE_NONE:
					piece_character = ' ';
			}

			if (piece_instance.piece->colour == COL_WHITE) {
				piece_character = static_cast<char>(toupper(piece_character));
			} else if (piece_instance.piece->colour == COL_BLACK) {
				piece_character = static_cast<char>(tolower(piece_character));
			}
			std::cout << piece_character;
		}
		std::cout << std::endl;
	}
}

std::string BoardState::get_fen() {
	std::string fen;
	for (uint8_t column = 0; column < 8; column++) {
		for (uint8_t row = 0; row < 8; row++) {
			const PieceInstance piece_instance = get_piece(row, column);
			char piece_character = ' ';
			switch (piece_instance.piece->type) {
				case PIECE_KING:
					piece_character = 'k';
				break;
				case PIECE_QUEEN:
					piece_character = 'q';
				break;
				case PIECE_ROOK:
					piece_character = 'r';
				break;
				case PIECE_BISHOP:
					piece_character = 'b';
				break;
				case PIECE_KNIGHT:
					piece_character = 'n';
				break;
				case PIECE_PAWN:
					piece_character = 'p';
				break;
				case PIECE_NONE:
					piece_character = '1';
					while (get_piece(row+1, column).piece->type == PIECE_NONE && (row + 1) < 8) {
						row++;
						piece_character++;
					}
			}

			if (piece_instance.piece->colour == COL_WHITE) {
				piece_character = static_cast<char>(toupper(piece_character));
			} else if (piece_instance.piece->colour == COL_BLACK) {
				piece_character = static_cast<char>(tolower(piece_character));
			}
			fen.push_back(piece_character);
		}
		if (!(column == 7)) {
			fen.push_back('/');
		}
	}
	fen.push_back(' ');

	fen.push_back(white_to_move ? 'w' : 'b');
	fen.push_back(' ');

	// Castling availability
	if (castling_state.white_kingside) {
		fen.push_back('K');
	}
	if (castling_state.white_queenside) {
		fen.push_back('Q');
	}
	if (castling_state.black_kingside) {
		fen.push_back('k');
	}
	if (castling_state.black_queenside) {
		fen.push_back('q');
	}
	if (castling_state.state == 0) {
		fen.push_back('-');
	}
	fen.push_back(' ');

	// En passant target square
	if (previous_move.has_value()) {
		if ((previous_move.value().new_position.get_bitboard_mask() & pieces_pawns).board != 0 &&
			previous_move.value().new_position.row == (white_to_move ? 4 : 3) && previous_move.value().old_position.row == (white_to_move ? 6 : 1)) {
			const SquarePosition en_passant_target_square{
				static_cast<uint8_t>(previous_move.value().old_position.row + (white_to_move ? -1 : 1)),
				previous_move.value().old_position.column
			};
			fen.push_back(en_passant_target_square.get_square_name().file);
			fen.push_back(en_passant_target_square.get_square_name().rank);
		} else {
			fen.push_back('-');
		}
	} else {
		fen.push_back('-');
	}

	// todo Halfmove clock
	// todo Fullmove number

	return fen;
}