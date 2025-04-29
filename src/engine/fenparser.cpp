#include "fenparser.h"

#include <cstring>

PieceColour char_to_piece(const char c) {
    switch (c) {
        case 'K':
            return {PIECE_KING, COL_WHITE};
        case 'k':
            return {PIECE_KING, COL_BLACK};
        case 'Q':
            return {PIECE_QUEEN, COL_WHITE};
        case 'q':
            return {PIECE_QUEEN, COL_BLACK};
        case 'R':
            return {PIECE_ROOK, COL_WHITE};
        case 'r':
            return {PIECE_ROOK, COL_BLACK};
        case 'B':
            return {PIECE_BISHOP, COL_WHITE};
        case 'b':
            return {PIECE_BISHOP, COL_BLACK};
        case 'N':
            return {PIECE_KNIGHT, COL_WHITE};
        case 'n':
            return {PIECE_KNIGHT, COL_BLACK};
        case 'P':
            return {PIECE_PAWN, COL_WHITE};
        case 'p':
            return {PIECE_PAWN, COL_BLACK};
        default:
            return {PIECE_NONE, COL_NONE};
    }
}

bool parse_fen_rank(const char* fen, FenRank* fen_rank) {
    if (fen_rank == nullptr) return false;

    int index = 0;
    int rank_index = 0;
    while (!(fen[index] == '\0' || fen[index] == '/' || fen[index] == ' ')) {
        const PieceColour piece = char_to_piece(fen[index]);

        if (piece.piece == PIECE_NONE) {
            // should be a number
            const uint8_t value = fen[index] - '0';
            for (uint8_t i = 0; i < value; i++) {
                fen_rank->pieces[rank_index + i] = PieceColour{PIECE_NONE, COL_NONE};
            }
            rank_index += value;
        } else {
            fen_rank->pieces[rank_index] = piece;
            rank_index++;
        }

        index++;
    }

    return true;
}

bool fen_parser(const std::string& fen, FenState& state) {
    int rank_index = 0;

    parse_fen_rank(&fen[0], &state.ranks[0]);
    rank_index++;
    int i = 0;

    while (i < fen.length()) {
        if (fen[i] == '/') {
            parse_fen_rank(&fen[i+1], &state.ranks[rank_index]);
            rank_index++;
        } else if (fen[i] == ' ') {
            parse_fen_rank(&fen[i+1], &state.ranks[rank_index]);
            i++;
            break;
        }

        i++;
    }

    state.is_white_to_move = (fen[i] == 'w');
    i += 2;

    if (fen[i] == '-') {
        state.castling.white_kingside  = true;
        state.castling.black_kingside  = true;
        state.castling.white_queenside = true;
        state.castling.black_queenside = true;
    } else {
        state.castling.white_kingside  = false;
        state.castling.black_kingside  = false;
        state.castling.white_queenside = false;
        state.castling.black_queenside = false;

        while (i < fen.length() && fen[i] != ' ') {
            switch (fen[i]) {
                case 'K':
                    state.castling.white_kingside = true;
                    break;
                case 'k':
                    state.castling.black_kingside = true;
                    break;
                case 'Q':
                    state.castling.white_queenside = true;
                    break;
                case 'q':
                    state.castling.black_queenside = true;
                    break;
                default:
                    break;
            }

            i++;
        }
    }
    i += 1;

    if (fen[i] != '-') {
        // en passant target square location
        const uint8_t row = fen[i] - 'a';
        i++;
        const uint8_t column = fen[i] - '0';

        const int forwards = (state.is_white_to_move ? -1 : 1);
        const SquarePosition old_position = {static_cast<uint8_t>(row - forwards), column};
        const SquarePosition new_position = {static_cast<uint8_t>(row + forwards), column};
        state.previous_move.emplace(Move {old_position, new_position});
    } else {
        state.previous_move.reset();
    }
    i += 2;

    uint8_t halfmove = 0;
    while (i < fen.length() && fen[i] != ' ') {
        const int digit = fen[i] - '0';
        halfmove *= 2;
        halfmove += digit;

        i++;
    }
    state.halfmove_clock = halfmove;
    i += 2;

    uint16_t fullmove = 0;
    while (i < fen.length() && fen[i] != ' ') {
        const int digit = fen[i] - '0';
        fullmove *= 2;
        fullmove += digit;

        i++;
    }
    state.fullmove_number = fullmove;

    return true;
}
