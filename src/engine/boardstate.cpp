#include "boardstate.h"

void BoardState::print() {
	for (uint8_t row = 7; row < 8; row--) {
		for (uint8_t column = 0; column < 8; column++) {
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

void BoardState::setup_default() {
	pieces_white = BitBoard(FIRST_RANK | SECOND_RANK);
	pieces_black = BitBoard(EIGHTH_RANK | SEVENTH_RANK);
	pieces_kings = BitBoard((FIRST_RANK | EIGHTH_RANK) & FIFTH_FILE);
	pieces_queens = BitBoard((FIRST_RANK | EIGHTH_RANK) & FOURTH_FILE);
	pieces_rooks = BitBoard((FIRST_RANK | EIGHTH_RANK) & (FIRST_FILE | EIGHTH_FILE));
	pieces_bishops = BitBoard((FIRST_RANK | EIGHTH_RANK) & (THIRD_FILE | SIXTH_FILE));
	pieces_knights = BitBoard((FIRST_RANK | EIGHTH_RANK) & (SECOND_FILE | SEVENTH_FILE));
	pieces_pawns = BitBoard(SECOND_RANK | SEVENTH_RANK);
	white_to_move = true;

	SET_PIECE_IN_ARRAY(pieces, KING, KING, 3);
	SET_PIECE_IN_ARRAY(pieces, QUEEN, QUEEN, 4);
	SET_PIECE_IN_ARRAY(pieces, ROOK_LEFT, ROOK, 0);
	SET_PIECE_IN_ARRAY(pieces, ROOK_RIGHT, ROOK, 7);
	SET_PIECE_IN_ARRAY(pieces, BISHOP_LEFT, BISHOP, 2);
	SET_PIECE_IN_ARRAY(pieces, BISHOP_RIGHT, BISHOP, 5);
	SET_PIECE_IN_ARRAY(pieces, KNIGHT_LEFT, KNIGHT, 1);
	SET_PIECE_IN_ARRAY(pieces, KNIGHT_RIGHT, KNIGHT, 6);

	for (uint8_t i = 0; i < 8; i++) {
		pieces[PIECE_ARRAY_WHITE_PAWN_LEFTMOST + i].piece = std::make_unique<Piece>(COL_WHITE, PIECE_PAWN);
		pieces[PIECE_ARRAY_WHITE_PAWN_LEFTMOST + i].position = SquarePosition{1, i};
		pieces[PIECE_ARRAY_BLACK_PAWN_LEFTMOST + i].piece = std::make_unique<Piece>(COL_BLACK, PIECE_PAWN);
		pieces[PIECE_ARRAY_BLACK_PAWN_LEFTMOST + i].position = SquarePosition{6, i};
	}
}

void BoardState::setup_from_fen(const FenState &fen) {
	uint8_t piece_index = 0;
	for (uint8_t rank_index = 0; rank_index < 8; rank_index++) {
		const FenRank rank = fen.ranks[rank_index];
		for (uint8_t column_index = 0; column_index < 8; column_index++) {
			const PieceColour piece = rank.pieces[column_index];
			const SquarePosition position = {static_cast<uint8_t>(7-rank_index), column_index};

			if (piece.piece == PIECE_NONE) continue;

			switch (piece.colour) {
				case COL_WHITE: pieces_white |= position.get_bitboard_mask(); break;
				case COL_BLACK: pieces_black |= position.get_bitboard_mask(); break;
				default: break;
			}

			switch (piece.piece) {
				case PIECE_KING:   pieces_kings   |= position.get_bitboard_mask(); break;
				case PIECE_QUEEN:  pieces_queens  |= position.get_bitboard_mask(); break;
				case PIECE_ROOK:   pieces_rooks   |= position.get_bitboard_mask(); break;
				case PIECE_BISHOP: pieces_bishops |= position.get_bitboard_mask(); break;
				case PIECE_KNIGHT: pieces_knights |= position.get_bitboard_mask(); break;
				case PIECE_PAWN:   pieces_pawns   |= position.get_bitboard_mask(); break;
				default: break;
			}

			pieces[piece_index].piece = std::make_unique<Piece>(piece.colour, piece.piece);
			pieces[piece_index].position = position;
			piece_index++;
		}
	}

	white_to_move = fen.is_white_to_move;
	castling_state = fen.castling;
	previous_move = fen.previous_move;

	//todo: fullmove and halfmove
}
