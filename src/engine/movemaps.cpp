#include "movemaps.h"

std::unordered_map<uint64_t, uint64_t> rank_masks = get_rank_mask();
std::unordered_map<uint64_t, uint64_t> file_masks = get_file_mask();
std::unordered_map<uint64_t, uint64_t> diag_masks_ne = get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> diag_masks_nw = get_diag_mask_nw();
std::unordered_map<uint16_t, uint8_t> rank_attacks_map = get_rank_attacks();
std::unordered_map<uint16_t, uint64_t> diag_attacks_ne = get_diag_attacks_ne_better();
std::unordered_map<uint16_t, uint64_t> diag_attacks_nw = get_diag_attacks_nw_better();


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

