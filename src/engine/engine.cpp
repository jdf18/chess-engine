#include "decisiontree.h"

#include <iostream>
#include <bitset>

#include "decisiontree.h"
#include "fenparser.h"

int main() {

    FenState fen_state;
    std::string fen;
    fen = "rnbqkbnr/pppp1pp1/8/3Pp2p/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 3";
    // std::getline(std::cin, fen);
    fen_parser(fen, fen_state);
    BoardState fen_board_state;
    fen_board_state.setup_from_fen(fen_state);
    fen_board_state.print();

    DecisionTree tree;
    tree.root = std::make_unique<DecisionTreeNode>(NodeData(fen_board_state));
    tree.root->data.board_state.print();


    std::cout << tree.root->children.size() << std::endl;

    tree.root->generate_moves();

    std::cout << std::endl;
    std::cout << tree.root->children.size() << std::endl;
    for (int i = 0; i < tree.root->children.size(); i++) {
        Move move = tree.root->children[i]->data.board_state.previous_move.value();
        std::cout << std::endl;
        move.print();
        std::cout << std::endl << tree.root->children[i]->data.board_state.white_to_move << std::endl;
        std::cout << std::endl;
        tree.root->children[i]->data.board_state.print();
    }

    return 0;
}
