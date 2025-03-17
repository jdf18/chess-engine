#include "boardstate.h"

#include <iostream>
#include <bitset>

BitBoard reverse_board(BitBoard to_reverse) {
    BitBoard rev = 0;

    for (int i = 0; i < 64; i++) {
        rev <<= 1;

        if ((to_reverse & 1) == 1) {
            rev ^= 1;
        }

        to_reverse >>= 1;
    }
    return rev;
}

static void out_bitboard(BitBoard to_out) {
    std::cout << std::bitset<64>(to_out).to_string() << std::endl;
    to_out = reverse_board(to_out);
    std::cout << std::bitset<64>(to_out).to_string() << std::endl;
    std::cout << std::endl;
    std::cout << std::bitset<8>(to_out & 0xFF).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF00) >> 8).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF0000) >> 16).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF000000) >> 24).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF00000000) >> 32).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF0000000000) >> 40).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF000000000000) >> 48).to_string() << std::endl;
    std::cout << std::bitset<8>((to_out & 0xFF00000000000000) >> 56).to_string() << std::endl;
}

int main() {
    BoardState initial_state;
    initial_state.pieces_knights = 0x0001000000001000;
    initial_state.pieces_white = initial_state.pieces_knights;

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knights_moves(COL_WHITE);

    out_bitboard(initial_state.pieces_knights);

    std::cout << std::endl;

    out_bitboard(possible_moves);


    return 0;
}