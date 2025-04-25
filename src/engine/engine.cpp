#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;
    initial_state.setup_default();
  
    initial_state.print();
    std::cout << std::endl << initial_state.get_fen();

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knight_moves(0x0000000000000002);

    initial_state.pieces_knights.out();

    std::cout << std::endl;

    possible_moves.out();

    std::cout << std::endl;

    BitBoard rook_pos = 0x0000000000000008;
    rook_pos.out();

    initial_state.pieces_white |= rook_pos;

    BitBoard rook_moves = initial_state.pseudo_legal_rook_moves(rook_pos);
    BitBoard bishop_moves = initial_state.pseudo_legal_bishop_moves(rook_pos);

    std::cout << std::endl;

    rook_moves.out();

    std::cout << std::endl;

    bishop_moves.out();

    return 0;
}