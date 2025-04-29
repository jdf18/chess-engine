#include "decisiontree.h"

void DecisionTreeNode::add_child(NodeData child_data) {
    children.push_back(std::make_unique<DecisionTreeNode>(child_data));
}

// Move{SquarePosition{0,4}, SquarePosition{0, 1}},
// Move{SquarePosition{0,4}, SquarePosition{0, 6}},
// Move{SquarePosition{7,4}, SquarePosition{7, 1}},
// Move{SquarePosition{7,4}, SquarePosition{7, 6}},

void DecisionTreeNode::generate_castle_moves() {
    Move possible_move = {SquarePosition{0,4},SquarePosition{0,0}};
    CastleType type;
    for (const Colour colour : {COL_WHITE, COL_BLACK}) {
        switch (colour) {
            case COL_WHITE:
                possible_move.old_position.row = 0;
                possible_move.new_position.row = 0;
                break;
            case COL_BLACK:
                possible_move.old_position.row = 7;
                possible_move.new_position.row = 7;
                break;
            default: break;
        }

        if (colour != (data.board_state.white_to_move ? COL_WHITE : COL_BLACK)) continue;

        for (const CastleSide side : {CASTLE_KINGSIDE, CASTLE_QUEENSIDE}) {
            switch (colour) {
                case CASTLE_KINGSIDE:
                    possible_move.new_position.column = 6;
                    break;
                case CASTLE_QUEENSIDE:
                    possible_move.new_position.column = 1;
                    break;
                default: break;
            }

            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            if (new_data.board_state.is_move_castle_valid(possible_move, CastleType{colour, side})) {
                // move pieces
                new_data.board_state.move_castle(CastleType{colour, side});

                // update castling state
                new_data.board_state.castling_state.remove_castle();

                add_child(new_data);
            }
        }
    }
}

void DecisionTreeNode::generate_en_passant_moves() {
    const BoardState* current_board = &data.board_state;

    // possible moves for en passant
    const PieceInstance previous_moved_piece = current_board->get_piece(current_board->previous_move->new_position);
    const bool passant_move_valid =
        (abs(current_board->previous_move->new_position.row - current_board->previous_move->old_position.row) == 2) &&
        (current_board->previous_move->new_position.column == current_board->previous_move->old_position.column);

    if (previous_moved_piece.piece->type == PIECE_PAWN && passant_move_valid) {
        const SquarePosition passant_taken_pawn = previous_moved_piece.position;
        const SquarePosition passant_end_square = {
            static_cast<uint8_t>(passant_taken_pawn.row + (current_board->white_to_move ? 1 : -1)),
            passant_taken_pawn.column};

        Move possible_moves[2] = {
            Move{SquarePosition{passant_taken_pawn.row, static_cast<uint8_t>(passant_taken_pawn.column-1)}, passant_end_square},
            Move{SquarePosition{passant_taken_pawn.row, static_cast<uint8_t>(passant_taken_pawn.column+1)}, passant_end_square}
        };

        for (const Move& possible_move : possible_moves) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            PieceInstance moving_piece = data.board_state.get_piece(possible_move.old_position);
            if (moving_piece.piece->colour != (data.board_state.white_to_move ? COL_WHITE : COL_BLACK)) continue;

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);
            new_data.board_state.remove_piece(passant_taken_pawn);

            add_child(new_data);
        }
    }
}


void DecisionTreeNode::generate_moves() {
    const BoardState* current_board = &data.board_state;

    generate_castle_moves();
    generate_en_passant_moves();

    for (uint8_t i = 0; i < 32; i++) {
        PieceInstance* piece_instance = &data.board_state.pieces[i];
        if (piece_instance->piece->colour != (data.board_state.white_to_move ? COL_WHITE : COL_BLACK)) continue;

        piece_instance->position.print();
        for (const Move& possible_move : piece_instance->generateMoves(current_board)) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);

            add_child(new_data);
        }
    }
}