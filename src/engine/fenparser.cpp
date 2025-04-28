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
    while (fen[index] != '\0' || fen[index] != '/' || fen[index] != ' ') {
        const PieceColour piece = char_to_piece(fen[index]);

        if (piece.piece == PIECE_NONE) {
            // should be a number
            const int value = fen[index] - '0';
            rank_index += value;
            // ? Might have to explicitly set each item to NONE
        } else {
            fen_rank->pieces[rank_index] = piece;
            rank_index++;
        }

        index++;
    }

    return true;
}

bool fen_parser(const std::string& fen, FenState* state) {
    int rank_index = 0;

    parse_fen_rank(&fen[0], &state->ranks[0]);
    rank_index++;
    int i = 0;

    while (i < fen.length()) {
        if (fen[i] == '/') {
            parse_fen_rank(&fen[i+1], &state->ranks[rank_index]);
            rank_index++;
        } else if (fen[i] == ' ') {
            parse_fen_rank(&fen[i+1], &state->ranks[rank_index]);
            i++;
            break;
        }

        i++;
    }

    return true;
}
