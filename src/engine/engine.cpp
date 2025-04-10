#include "boardstate.h"

#include <iostream>
#include <bitset>

int main() {
    BoardState initial_state;
    initial_state.setup_default();

    initial_state.print();
    std::cout << std::endl << initial_state.get_fen();

    return 0;
}