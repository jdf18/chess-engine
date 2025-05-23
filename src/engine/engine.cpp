#include "decisiontree.h"

#include <iostream>
#include <bitset>

#include "fenparser.h"
#include "pieces.h"

int main() {
    FenState fen_state;
    std::string fen;
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // std::getline(std::cin, fen);
    fen_parser(fen, fen_state);
    BoardState fen_board_state;
    fen_board_state.setup_from_fen(fen_state);
    std::cout << "INITIAL BOARD STATE" << std::endl;
    fen_board_state.print();
    std::cout << std::endl;

    DecisionTree tree;
    tree.root = std::make_unique<DecisionTreeNode>(NodeData(fen_board_state));
    MoveEvaluated move = tree.root.get()->return_best_move(5);
    if (move.mate) {
        std::cout << "mate" << std::endl;
    }
    else {
        move.move.value().print();
    }

    int x;
    std::cin >> x;
    std::cout << x;
    return 0;
}
