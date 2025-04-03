#include "decisiontree.h"

bool NodeData::initialise_node() {
    // NodeData attributes: previous_move; board_state;

    // todo: generate the bitboards and test to see if the king is in check
    // todo:   also check to see if anyone has won and set a flag on the node.

    return true;
}


void DecisionTreeNode::add_child(NodeData child_data) {
    children.push_back(std::make_unique<DecisionTreeNode>(child_data));
}

void DecisionTreeNode::generate_moves() {
    BoardState current_board = data.board_state;
    // for piece in board.
    //     for possible move in piece
    //         new_data.previous_move = ...;
    //         new_data.board_state = current_board;
    //         update the board state
    //         add_child(new_data)
    for (PieceInstance piece_instance : current_board.pieces) {
        for (Move possible_move : piece_instance.generateMoves(current_board)) {
            NodeData new_data{current_board, possible_move};
            if (new_data.initialise_node()) {
                add_child(new_data);
            }
            //todo: modify this to reflect the change
        }
    }
}