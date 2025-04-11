#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;
    initial_state.setup_default();
  
    initial_state.print();
    std::cout << std::endl << initial_state.get_fen();

    std::cout << "\n" << std::endl;

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knights_moves(COL_WHITE);

    initial_state.pieces_knights.out();

    std::cout << std::endl;

    possible_moves.out();

    std::cout << std::endl;

    return 0;
}