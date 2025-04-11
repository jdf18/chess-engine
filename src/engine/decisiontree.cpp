#include "decisiontree.h"

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
    for (PieceInstance piece : current_board.pieces) {
        for (Move possible_move : piece.generateMoves(current_board)) {
            NodeData new_data{current_board, possible_move};
            //todo: modify this to reflect the change
        }
    }
}