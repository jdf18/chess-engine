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
		diag &= ((square & BOARD_WHITE_SQUARE_MASK) != 0) ? BOARD_WHITE_SQUARE_MASK : BOARD_BLACK_SQUARE_MASK;
		mask.insert({ square, diag });
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
	}
	return mask;
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
			uint16_t key = (square << 8) | occupancy;
			map.insert({ key, attacks });
		}
		square++;
	}
	return map;
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