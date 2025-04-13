#include "decisiontree.h"

#include <iostream>
#include <bitset>

#include "decisiontree.h"

int main() {
    BoardState initial_board_state;
    initial_board_state.setup_default();

    DecisionTree tree;
    tree.root = std::make_unique<DecisionTreeNode>(NodeData(initial_board_state));
    tree.root->data.board_state.print();

    std::cout << tree.root->children.size() << std::endl;

    tree.root->generate_moves();

    std::cout << tree.root->children.size() << std::endl;
    for (int i = 0; i < tree.root->children.size(); i++) {
        Move move = tree.root->children[i]->data.board_state.previous_move.value();
        std::cout << std::endl;
        move.print();
        std::cout << std::endl;
        tree.root->children[i]->data.board_state.print();
    }

    BitBoard possible_moves;
    possible_moves = initial_state.pseudo_legal_knight_moves(COL_WHITE);

    initial_state.pieces_knights.out();

    std::cout << std::endl;

    possible_moves.out();

    std::cout << std::endl;

    BitBoard rook_pos = 0x0000000000000008;
    rook_pos.out();

    initial_state.pieces_white |= rook_pos;

    BitBoard rook_moves = initial_state.pseudo_legal_rook_moves(COL_WHITE, rook_pos);
    BitBoard bishop_moves = initial_state.pseudo_legal_bishop_moves(COL_WHITE, rook_pos);

    std::cout << std::endl;

    rook_moves.out();

    std::cout << std::endl;

    bishop_moves.out();

    return 0;
}
