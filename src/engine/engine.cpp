#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;

    initial_state.pieces_white = FIRST_RANK | SECOND_RANK;
    initial_state.pieces_black = EIGHTH_RANK | SEVENTH_RANK;

    initial_state.pieces_kings = (FIRST_RANK | EIGHTH_RANK) & FIFTH_FILE;
    initial_state.pieces_queens = (FIRST_RANK | EIGHTH_RANK) & FOURTH_FILE;
    initial_state.pieces_rooks = (FIRST_RANK | EIGHTH_RANK) & (FIRST_FILE | EIGHTH_FILE);
    initial_state.pieces_bishops = (FIRST_RANK | EIGHTH_RANK) & (THIRD_FILE | SIXTH_FILE);
    initial_state.pieces_knights = (FIRST_RANK | EIGHTH_RANK) & (SECOND_FILE | SEVENTH_FILE);
    initial_state.pieces_pawns = SECOND_RANK | SEVENTH_RANK;

    initial_state.print();
    std::cout << std::endl << initial_state.get_fen();

    return 0;
}