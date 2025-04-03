#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;
    initial_state.pieces_knights = 0x0001000000001000;
    initial_state.pieces_white = initial_state.pieces_knights;

    BitBoard possible_moves = initial_state.pseudo_legal_knights_moves(COL_WHITE);

    initial_state.pieces_knights.out();

    std::cout << std::endl;

    possible_moves.out();

    std::cout << std::endl;

    std::unordered_map<uint16_t, uint8_t> rank_attacks = get_rank_attacks();

    std::cout << std::bitset<8>(rank_attacks[0x08C3]).to_string() << std::endl;

    return 0;
}