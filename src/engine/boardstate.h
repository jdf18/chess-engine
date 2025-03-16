#include <cstdint>

#include "pieces.h"

#define BOARD_ROW 8
#define BOARD_COL 1

#define BOARD_WHITE_SQUARE_MASK 0x55AA55AA55AA55AA
#define BOARD_BLACK_SQUARE_MASK ~BOARD_WHITE_SQUARE_MASK

#define FIRST_RANK 0xFF
#define SECOND_RANK 0xFF00
#define THIRD_RANK 0xFF0000
#define FOURTH_RANK 0xFF000000
#define FIFTH_RANK 0xFF00000000
#define SIXTH_RANK 0xFF0000000000
#define SEVENTH_RANK 0xFF000000000000
#define EIGHTH_RANK 0xFF00000000000000

#define FIRST_COL 0x0101010101010101
#define SECOND_COL 0x0202020202020202
#define THIRD_COL 0x0404040404040404
#define FOURTH_COL 0x0808080808080808
#define FIFTH_COL 0x1010101010101010
#define SIXTH_COL 0x2020202020202020
#define SEVENTH_COL 0x4040404040404040
#define EIGHTH_COL 0x8080808080808080


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

typedef uint64_t BitBoard;

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