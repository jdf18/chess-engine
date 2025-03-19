#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;
    initial_state.pieces_knights = 0x0001000000001000;
    initial_state.pieces_white = initial_state.pieces_knights;

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knights_moves(COL_WHITE);

    initial_state.pieces_knights.out();

    std::cout << std::endl;

    possible_moves.out();


    return 0;
}