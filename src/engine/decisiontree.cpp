#include "decisiontree.h"

void DecisionTreeNode::add_child(NodeData child_data) {
    children.push_back(std::make_unique<DecisionTreeNode>(child_data));
}

void DecisionTreeNode::generate_moves() {
    const BoardState* current_board = &data.board_state;

    // possible moves for castling
    for (CastleType castle_type = CASTLE_WHITE_KINGSIDE; castle_type != CASTLE_BLACK_QUEENSIDE; castle_type++) {
        const Move possible_move = castle_moves[castle_type];
        NodeData new_data{data.board_state};
        new_data.board_state.previous_move = possible_move;

        if (new_data.board_state.is_move_castle_valid(possible_move)) {
            // move pieces
            new_data.board_state.move_castle(castle_type);

            // update castling state
            new_data.board_state.castling_state.remove_castle();
        }

        add_child(new_data);
    }



    for (uint8_t i = 0; i < 31; i++) {
        PieceInstance* piece_instance = &data.board_state.pieces[i];
        piece_instance->position.print();
        for (const Move& possible_move : piece_instance->generateMoves(current_board)) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);

            add_child(new_data);
        }
    }
}