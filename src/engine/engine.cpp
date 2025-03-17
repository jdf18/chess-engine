#include "boardstate.h"

#include <iostream>

int main() {
    BoardState initial_state;
    initial_state.pieces_knights = 0x0001000000001000;
    initial_state.pieces_white = initial_state.pieces_knights;

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knights_moves(COL_WHITE);

    std::cout << (uint64_t)initial_state.pieces_knights << '\n';
    std::cout << (uint64_t)possible_moves << '\n';

    return 0;
}