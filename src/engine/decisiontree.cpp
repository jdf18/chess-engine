#include "decisiontree.h"

void DecisionTreeNode::add_child(NodeData child_data) {
    children.push_back(std::make_unique<DecisionTreeNode>(child_data));
}

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
    if (!(current_board->previous_move.has_value())) {
        return;
    }
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

void DecisionTreeNode::generate_promotion_moves(NodeData board_data, const SquarePosition pawn_position) {
    for (Pieces type : {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT}) {
        board_data.board_state.promote_piece(pawn_position, type);
        board_data.board_state.previous_move.value().piece_promotion = type;
        add_child(board_data);
    }
}

void DecisionTreeNode::generate_moves() {
    if (processed == NODE_CHILDREN_GENERATED) return;
    // Don't bother generating moves if the parent move was illegal
    if (data.legality == NODE_ILLEGAL) return;

    const BoardState* current_board = &data.board_state;

    generate_castle_moves();
    generate_en_passant_moves();

    for (uint8_t i = 0; i < 32; i++) {
        PieceInstance* piece_instance = &data.board_state.pieces[i];
        if (piece_instance->piece->colour != (data.board_state.white_to_move ? COL_WHITE : COL_BLACK)) continue;

        for (const Move& possible_move : piece_instance->generateMoves(current_board)) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);

            if (piece_instance->piece->type == PIECE_PAWN && (piece_instance->position.row == 0 || piece_instance->position.row == 7)) {
                generate_promotion_moves(new_data, piece_instance->position);
            } else add_child(new_data);
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


//Sum up the material for a given colour.
//Used the AlphaZero values for the pieces, rather than the typical 1-3-3-5-9 values. (from this paper: https://arxiv.org/abs/2009.04374)
//This evaluates a bishop to be more powerful than a knight (which is usually, but not always, true).
//It also evaluates a rook and queen to be noticaebly more powerful.
float NodeData::material_sum(Colour col) {
    BitBoard col_pieces = col == COL_WHITE ? board_state.pieces_white : board_state.pieces_black;
    float result = 0;
    result += (col_pieces & board_state.pieces_kings).count_set_bits() * 1000; // King value
    result += (col_pieces & board_state.pieces_queens).count_set_bits() * 9.5f; // Queen value
    result += (col_pieces & board_state.pieces_rooks).count_set_bits() * 5.63f; // Rook value
    result += (col_pieces & board_state.pieces_knights).count_set_bits() * 3.05f; // Knight value
    result += (col_pieces & board_state.pieces_bishops).count_set_bits() * 3.33f; // Bishop value
    result += (col_pieces & board_state.pieces_pawns).count_set_bits() * 1; // Pawn value
    return result;
}

float NodeData::evaluate() {
    return material_sum(COL_WHITE) - material_sum(COL_BLACK);
}


//*Should* return the best move for the colour whose turn it is for this node.
MoveEvaluated DecisionTreeNode::return_best_move(uint8_t depth) {
    MoveEvaluated result;
    //Base case
    if (depth == 0) {
        result.evaluation = data.evaluate();
        return result;
    }

    //If not the base case
    else {
        //Get all possible moves in this position
        generate_moves();
        std::vector<MoveEvaluated> evaluated_moves;
        evaluated_moves.reserve(children.size());

        //For each possible move, store the best move for the OTHER colour in the position reached if we make the move.
        for (std::unique_ptr<DecisionTreeNode>& child : children) {
            evaluated_moves.push_back(child.get()->return_best_move(depth - 1));
        }

        //If it is white's turn to move, choose the move which gives the maximum evaluation
        if (data.board_state.white_to_move) {
            //Initialise the evaluation to negative infinity, so anything is better than it
            result.evaluation = -INFINITY;
            //For every move, if its evaluation is greater than the current result's evaluation, choose it over the current result
            for (int i = 0; i < evaluated_moves.size(); i++) {
                if (evaluated_moves[i].evaluation > result.evaluation) {
                    result.evaluation = evaluated_moves[i].evaluation;
                    result.move = children[i].get()->data.board_state.previous_move;
                }
            }
            return result;
        }
        result.evaluation = INFINITY;
        for (int i = 0; i < evaluated_moves.size(); i++) {
            if (evaluated_moves[i].evaluation < result.evaluation) {
                result.evaluation = evaluated_moves[i].evaluation;
                result.move = children[i].get()->data.board_state.previous_move;
            }
        }
        return result;
    }
}