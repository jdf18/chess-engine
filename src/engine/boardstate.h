#include <cstdint>
#include <unordered_map>
#include <iostream>
#include <bitset>

#include "pieces.h"
#include "bitboard.h"

std::unordered_map<uint64_t, uint64_t> get_file_mask();
std::unordered_map<uint64_t, uint64_t> get_rank_mask();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_ne();
std::unordered_map<uint64_t, uint64_t> get_diag_mask_nw();

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

struct BoardState {
    BitBoard pieces_white;
    BitBoard pieces_black;

    BitBoard pieces_kings;
    BitBoard pieces_queens;
    BitBoard pieces_rooks;
    BitBoard pieces_knights;
    BitBoard pieces_bishops;
    BitBoard pieces_pawns;

    // TODO: possibly add a uint8_t for if a pawn moved 2 spaces in 
    //   that column on the last move, to check if en-passant is possible

    // TODO: track castling rights (if the rooks have ever moved, or king)
    //   uint8_t (4 bits needed, one for each possible castle)

    // Returns true if there is a piece on the square 
    //   (or any of the squares) given by bitboard 'square'.
    inline bool is_piece_on_square_mask(BitBoard squares) {
        return (squares & (pieces_white | pieces_black)) != 0;
    };

    BitBoard pseudo_legal_king_moves(Colour colour);
    BitBoard pseudo_legal_queen_moves(Colour colour);
    BitBoard pseudo_legal_rook_moves(Colour colour);
    BitBoard pseudo_legal_knights_moves(Colour colour);
    BitBoard pseudo_legal_bishop_moves(Colour colour);
    BitBoard pseudo_legal_pawn_moves(Colour colour);
};

