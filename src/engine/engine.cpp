#include "decisiontree.h"

#include <iostream>
#include <bitset>

#include "decisiontree.h"
#include "fenparser.h"

std::string get_best_move(std::string fen) {
    FenState fen_state;
    fen_parser(fen, fen_state);
    BoardState board_state;
    board_state.setup_from_fen(fen_state);

    DecisionTree tree;
    tree.root = std::make_unique<DecisionTreeNode>(NodeData(board_state));

    return ""; //todo: link to minmax algorithm later
}

// int main() {
//     FenState fen_state;
//     std::string fen;
//     fen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 0";
//     // std::getline(std::cin, fen);
//     fen_parser(fen, fen_state);
//     BoardState fen_board_state;
//     fen_board_state.setup_from_fen(fen_state);
//     std::cout << "INITIAL BOARD STATE" << std::endl;
//     fen_board_state.print();
//     std::cout << std::endl;
//
//     DecisionTree tree;
//     tree.root = std::make_unique<DecisionTreeNode>(NodeData(fen_board_state));
//
//     tree.root->generate_moves();
//     // tree.root->generate_castle_moves();
//
//     std::cout << "GENERATED MOVES";
//     for (int i = 0; i < tree.root->children.size(); i++) {
//         Move move = tree.root->children[i]->data.board_state.previous_move.value();
//         move.print();
//         std::cout << std::endl;
//         tree.root->children[i]->data.board_state.print();
//         std::cout << std::endl;
//     }
//
//     return 0;
// }
