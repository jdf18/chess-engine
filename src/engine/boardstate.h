#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include <optional>

#include "pieces.h"
#include "piece.h"
#include "bitboard.h"

#define PIECE_ARRAY_KING 0
#define PIECE_ARRAY_QUEEN 1
#define PIECE_ARRAY_ROOK_LEFT 2
#define PIECE_ARRAY_ROOK_RIGHT 3
#define PIECE_ARRAY_BISHOP_LEFT 4
#define PIECE_ARRAY_BISHOP_RIGHT 5
#define PIECE_ARRAY_KNIGHT_LEFT 6
#define PIECE_ARRAY_KNIGHT_RIGHT 7
#define PIECE_ARRAY_PAWN_LEFTMOST 8
#define PIECE_ARRAY_PAWN_RIGHTMOST 15

#define PIECE_ARRAY_WHITE_OFFSET 0
#define PIECE_ARRAY_WHITE_KING              (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_KING)
#define PIECE_ARRAY_WHITE_QUEEN             (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_QUEEN)
#define PIECE_ARRAY_WHITE_ROOK_LEFT         (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_ROOK_LEFT)
#define PIECE_ARRAY_WHITE_ROOK_RIGHT        (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_ROOK_RIGHT)
#define PIECE_ARRAY_WHITE_BISHOP_LEFT       (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_BISHOP_LEFT)
#define PIECE_ARRAY_WHITE_BISHOP_RIGHT      (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_BISHOP_RIGHT)
#define PIECE_ARRAY_WHITE_KNIGHT_LEFT       (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_KNIGHT_LEFT)
#define PIECE_ARRAY_WHITE_KNIGHT_RIGHT      (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_KNIGHT_RIGHT)
#define PIECE_ARRAY_WHITE_PAWN_LEFTMOST     (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_PAWN_LEFTMOST)
#define PIECE_ARRAY_WHITE_PAWN_RIGHTMOST    (PIECE_ARRAY_WHITE_OFFSET + PIECE_ARRAY_PAWN_RIGHTMOST)

#define PIECE_ARRAY_BLACK_OFFSET 16
#define PIECE_ARRAY_BLACK_KING              (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_KING)
#define PIECE_ARRAY_BLACK_QUEEN             (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_QUEEN)
#define PIECE_ARRAY_BLACK_ROOK_LEFT         (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_ROOK_LEFT)
#define PIECE_ARRAY_BLACK_ROOK_RIGHT        (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_ROOK_RIGHT)
#define PIECE_ARRAY_BLACK_BISHOP_LEFT       (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_BISHOP_LEFT)
#define PIECE_ARRAY_BLACK_BISHOP_RIGHT      (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_BISHOP_RIGHT)
#define PIECE_ARRAY_BLACK_KNIGHT_LEFT       (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_KNIGHT_LEFT)
#define PIECE_ARRAY_BLACK_KNIGHT_RIGHT      (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_KNIGHT_RIGHT)
#define PIECE_ARRAY_BLACK_PAWN_LEFTMOST     (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_PAWN_LEFTMOST)
#define PIECE_ARRAY_BLACK_PAWN_RIGHTMOST    (PIECE_ARRAY_BLACK_OFFSET + PIECE_ARRAY_PAWN_RIGHTMOST)

#define SET_PIECE_IN_ARRAY(arr, offset, type, file) \
do { \
    arr[PIECE_ARRAY_WHITE_##offset].piece = std::make_unique<Piece>(COL_WHITE, PIECE_##type); \
    arr[PIECE_ARRAY_WHITE_##offset].position = SquarePosition{0, file}; \
    arr[PIECE_ARRAY_BLACK_##offset].piece = std::make_unique<Piece>(COL_BLACK, PIECE_##type); \
    arr[PIECE_ARRAY_BLACK_##offset].position = SquarePosition{7, file}; \
}while(0)

std::unordered_map<uint64_t, uint64_t> get_file_mask();
std::unordered_map<uint64_t, uint64_t> get_rank_mask();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_nw();
std::unordered_map<uint16_t, uint8_t> get_rank_attacks();
std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_ne();
std::unordered_map<std::bitset<128>, uint64_t> get_diag_attacks_nw();
std::unordered_map<uint16_t, uint64_t> get_diag_attacks_ne_better();
std::unordered_map<uint16_t, uint64_t> get_diag_attacks_nw_better();
std::unordered_map<uint64_t, int> get_square_to_index_map();




// bitboards: 64 bit unsigned integers, with 1 bit for each square of the board.
//   There will be quite a few different bitboards, containing useful information.
//   The least significant bit is the square a1. The next least significant is b1, 
//     so on and so forth. The ninth least significant will be a2. And the most 
//     significant is h8.
//   This means that a left shift of 1 corresponds to moving right one square, 
//     and a right shift of 1 corresponds to moving left one square. A left shift 
//     of 8 corresponds to moving up one square, and a right shift of 8 corresponds 
//     to moving down one square.

// Bitboard list:
//     pieces_white : Has a bit where all white pieces are.
//     pieces_black : Has a bit where all black pieces are.
//
//     pieces_kings: Has a bit where all the kings are.
//     pieces_queens: Has a bit where all the queens are.
//     pieces_rooks: Has a bit where all the rooks are.
//     pieces_knights: Has a bit where all the knights are.
//     pieces_bishops: Has a bit where all the bishops are.
//     pieces_pawns: Has a bit where all the pawns are.

class PieceInstance;

struct CastlingAvailability {
    union {
        struct {
            bool white_kingside;
            bool white_queenside;
            bool black_kingside;
            bool black_queenside;
        };
        std::bitset<4> state;
    };

    CastlingAvailability() {
        white_kingside = true;
        white_queenside = true;
        black_kingside = true;
        black_queenside = true;
    }

    CastlingAvailability(const CastlingAvailability& copy) {
        state = copy.state;
    }
};

struct BoardState {
    PieceInstance pieces[36];

    BitBoard pieces_white;
    BitBoard pieces_black;

    BitBoard pieces_kings;
    BitBoard pieces_queens;
    BitBoard pieces_rooks;
    BitBoard pieces_knights;
    BitBoard pieces_bishops;
    BitBoard pieces_pawns;

    bool white_to_move;

    CastlingAvailability castling_state;

    std::optional<Move> previous_move;

    // todo: 50 move rule

    // todo: check for repeated positions

    // Returns true if there is a piece on the square 
    //   (or any of the squares) given by bitboard 'square'.
    inline bool is_piece_on_square_mask(BitBoard squares) {
        return (squares & (pieces_white | pieces_black)).board != 0;
    };

    BitBoard pseudo_legal_king_moves(Colour colour);
    BitBoard pseudo_legal_queen_moves(Colour colour, BitBoard square);
    BitBoard pseudo_legal_rook_moves(Colour colour, BitBoard square);
    BitBoard pseudo_legal_knight_moves(Colour colour);
    BitBoard pseudo_legal_bishop_moves(Colour colour, BitBoard square);
    BitBoard pseudo_legal_pawn_moves(Colour colour);

    PieceInstance get_piece(uint8_t row, uint8_t column);

    void print();
    std::string get_fen();

    BoardState(const BoardState& copy) :
        pieces_white(copy.pieces_white),
        pieces_black(copy.pieces_black),
        pieces_kings(copy.pieces_kings),
        pieces_queens(copy.pieces_queens),
        pieces_rooks(copy.pieces_rooks),
        pieces_bishops(copy.pieces_bishops),
        pieces_knights(copy.pieces_knights),
        pieces_pawns(copy.pieces_pawns),
        castling_state(copy.castling_state),
        white_to_move(copy.white_to_move),
        previous_move(copy.previous_move)
    {
        for (int i = 0; i < 31; i++) {
            pieces[i].piece = std::make_unique<Piece>(copy.pieces[i].piece->colour, copy.pieces[i].piece->type);
            pieces[i].position = copy.pieces[i].position;
        }
    };
    BoardState() :
        pieces_white(0),
        pieces_black(0),
        pieces_kings(0),
        pieces_queens(0),
        pieces_rooks(0),
        pieces_bishops(0),
        pieces_knights(0),
        pieces_pawns(0),
        white_to_move(true) {}

    void setup_default() {
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

        for (uint8_t i = 0; i < 7; i++) {
            pieces[PIECE_ARRAY_WHITE_PAWN_LEFTMOST + i].piece = std::make_unique<Piece>(COL_WHITE, PIECE_PAWN);
            pieces[PIECE_ARRAY_WHITE_PAWN_LEFTMOST + i].position = SquarePosition{1, i};
            pieces[PIECE_ARRAY_BLACK_PAWN_LEFTMOST + i].piece = std::make_unique<Piece>(COL_BLACK, PIECE_PAWN);
            pieces[PIECE_ARRAY_BLACK_PAWN_LEFTMOST + i].position = SquarePosition{6, i};
        }
    }
};

#endif //BOARDSTATE_H