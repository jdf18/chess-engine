#include "decisiontree.h"

void DecisionTreeNode::add_child(NodeData child_data) {
    children.push_back(std::make_unique<DecisionTreeNode>(child_data));
}

void DecisionTreeNode::generate_castle_moves() {
    const BoardState* current_board = &data.board_state;

    // possible moves for castling
    for (int castle_type_num = CASTLE_WHITE_KINGSIDE; castle_type_num != CASTLE_BLACK_QUEENSIDE; castle_type_num++) {
        const Move possible_move = castle_moves[castle_type_num];
        NodeData new_data{data.board_state};
        new_data.board_state.previous_move = possible_move;

        if (new_data.board_state.is_move_castle_valid(possible_move)) {
            // move pieces
            new_data.board_state.move_castle(static_cast<CastleType>(castle_type_num));

            // update castling state
            if (current_board->white_to_move) {
                new_data.board_state.castling_state.white_kingside = false;
                new_data.board_state.castling_state.white_queenside = false;
            } else {
                new_data.board_state.castling_state.black_kingside = false;
                new_data.board_state.castling_state.black_queenside = false;
            }


            add_child(new_data);
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

            // new_data.board_state.is_move_en_passant_valid(possible_move)

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);
            new_data.board_state.remove_piece(passant_taken_pawn);

            add_child(new_data);
        }
    }
}


void DecisionTreeNode::generate_moves() {
    if (processed == NODE_CHILDREN_GENERATED) return;
    // Don't bother generating moves if the parent move was illegal
    if (data.legality == NODE_ILLEGAL) return;

    const BoardState* current_board = &data.board_state;

    generate_castle_moves();
    generate_en_passant_moves();

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
    processed = NODE_CHILDREN_GENERATED;
}

void DecisionTreeNode::check_legality() {
    // Dont redo everything if weve already worked this out
    if (data.legality != NODE_PSEUDO_LEGAL) return;

    const Colour enemy_colour = (data.last_player == COL_WHITE ? COL_BLACK : COL_WHITE);
    BitBoard enemy_attack_surface = 0;
    enemy_attack_surface |= data.board_state.pseudo_legal_king_moves(enemy_colour);
    // enemy_attack_surface |= data.board_state.pseudo_legal_queen_moves(enemy_colour);
    // enemy_attack_surface |= data.board_state.pseudo_legal_rook_moves(enemy_colour);
    // enemy_attack_surface |= data.board_state.pseudo_legal_bishop_moves(enemy_colour);
    enemy_attack_surface |= data.board_state.pseudo_legal_knight_moves(enemy_colour);
    enemy_attack_surface |= data.board_state.pseudo_legal_pawn_moves(enemy_colour);

    if ((enemy_attack_surface & data.board_state.pieces_kings &
        (data.last_player == COL_WHITE ? data.board_state.pieces_white : data.board_state.pieces_black)).board == 0) {
        data.legality = NODE_LEGAL;
    } else {
        data.legality = NODE_ILLEGAL;
    };
}
