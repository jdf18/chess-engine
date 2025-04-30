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
            if (possible_move.old_position.column > 7) continue;
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            PieceInstance moving_piece = data.board_state.get_piece(possible_move.old_position);
            if (moving_piece.piece->colour == COL_NONE) continue;
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

        for (const Move& possible_move : piece_instance->generateMoves(current_board)) {
            NodeData new_data{data.board_state};
            new_data.board_state.previous_move = possible_move;
            new_data.board_state.switch_turn();

            new_data.board_state.move_piece(possible_move.old_position, possible_move.new_position);

            add_child(new_data);
        }
    }
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
    result.lost_white_king = false;
    result.lost_black_king = false;
    //If there is no king in the current state, set result.lost_king to true.
    if (data.board_state.white_to_move) {
        if ((data.board_state.pieces_kings & data.board_state.pieces_white) == 0) {
            result.lost_white_king = true;
        }
    }
    else {
        if ((data.board_state.pieces_kings & data.board_state.pieces_black) == 0) {
            result.lost_black_king = true;
        }
    }
    //Base case
    if (depth == 0) {
        result.evaluation = data.evaluate();
        return result;
    }

    //If not the base case
    else {

        //This may have already been called at a depth of 1, so don't get children again if we already have them.
        if (children.size() == 0) {
            //Get all possible moves in this position
            generate_moves();
        }

        //Check which of the moves are actually valid, but only if the depth is greater than 1 (otherwise we get a stack overflow)
        std::vector<int> valid_indices;
        if (depth > 1) {
            std::vector<MoveEvaluated> next_moves;
            next_moves.reserve(children.size());

            for (std::unique_ptr<DecisionTreeNode>& child : children) {
                next_moves.push_back(child.get()->return_best_move(1));
            }

            valid_indices.reserve(children.size());
            for (int i = 0; i < next_moves.size(); i++) {
                MoveEvaluated move = next_moves[i];
                if (!(data.board_state.white_to_move ? move.lost_white_king : move.lost_black_king)) {
                    valid_indices.push_back(i);
                }
            }
        }
        else {
            valid_indices.reserve(children.size());
            for (int i = 0; i < children.size(); i++) {
                valid_indices.push_back(i);
            }
        }

        if (valid_indices.size() == 0) {
            result.mate = true;
            result.evaluation = data.board_state.white_to_move ? -2000 : 2000;
            (data.board_state.white_to_move ? result.lost_white_king : result.lost_black_king) = true;
            return result;
        }

        std::vector<MoveEvaluated> evaluated_moves;
        evaluated_moves.reserve(valid_indices.size());

        //For each possible move, store the best move for the OTHER colour in the position reached if we make the move.
        for (int i : valid_indices) {
            std::unique_ptr<DecisionTreeNode>& child = children[i];
            evaluated_moves.push_back(child.get()->return_best_move(depth - 1));
            if (data.board_state.white_to_move ? evaluated_moves.back().lost_black_king : evaluated_moves.back().lost_white_king) {
                break;
            }
        }

        //If it is white's turn to move, choose the move which gives the maximum evaluation
        if (data.board_state.white_to_move) {
            //Initialise the evaluation to negative infinity, so anything is better than it
            result.evaluation = -3000;
            //For every move, if its evaluation is greater than the current result's evaluation, choose it over the current result
            for (int i = 0; i < evaluated_moves.size(); i++) {
                if (evaluated_moves[i].evaluation > result.evaluation) {
                    result.evaluation = evaluated_moves[i].evaluation;
                    result.move = children[i].get()->data.board_state.previous_move;
                    result.lost_white_king = evaluated_moves[i].lost_white_king;
                    result.lost_black_king = evaluated_moves[i].lost_black_king;
                    result.mate = evaluated_moves[i].mate;
                }
            }
            return result;
        }
        result.evaluation = 3000;
        for (int i = 0; i < evaluated_moves.size(); i++) {
            if (evaluated_moves[i].evaluation < result.evaluation) {
                result.evaluation = evaluated_moves[i].evaluation;
                result.move = children[i].get()->data.board_state.previous_move;
                result.lost_white_king = evaluated_moves[i].lost_white_king;
                result.lost_black_king = evaluated_moves[i].lost_black_king;
                result.mate = evaluated_moves[i].mate;
            }
        }
        return result;
    }
}