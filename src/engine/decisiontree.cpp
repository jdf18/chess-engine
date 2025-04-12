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
    const BoardState* current_board = &data.board_state;

    for (uint8_t i = 0; i < 31; i++) {
        PieceInstance* piece_instance = &data.board_state.pieces[i];
        piece_instance->position.print();
        for (const Move& possible_move : piece_instance->generateMoves(current_board)) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            const BitBoard move_mask = (possible_move.old_position.get_bitboard_mask() | possible_move.new_position.get_bitboard_mask());

            if (piece_instance->piece->colour == COL_WHITE) {
                new_data.board_state.pieces_white ^= move_mask;
            } else {
                new_data.board_state.pieces_black ^= move_mask;
            }

            switch (piece_instance->piece->type) {
                case PIECE_KING:
                    new_data.board_state.pieces_kings ^= move_mask;
                    break;
                case PIECE_QUEEN:
                    new_data.board_state.pieces_queens ^= move_mask;
                    break;
                case PIECE_ROOK:
                    new_data.board_state.pieces_rooks ^= move_mask;
                    break;
                case PIECE_BISHOP:
                    new_data.board_state.pieces_bishops ^= move_mask;
                    break;
                case PIECE_KNIGHT:
                    new_data.board_state.pieces_knights ^= move_mask;
                    break;
                case PIECE_PAWN:
                    new_data.board_state.pieces_pawns ^= move_mask;
                    break;
                default: break;
            }

            if (new_data.initialise_node()) {
                add_child(new_data);
            }
        }
    }
}