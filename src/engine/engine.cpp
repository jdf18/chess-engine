#include "decisiontree.h"

#include <iostream>

int main() {
    BoardState initial_board_state;
    initial_board_state.pieces[PIECE_ARRAY_WHITE_KING].piece =
        std::make_unique<Piece>(COL_WHITE, PIECE_KING);
    initial_board_state.pieces[PIECE_ARRAY_WHITE_KING].position = SquarePosition{0, 2};

    initial_board_state.pieces[PIECE_ARRAY_BLACK_KING].piece =
        std::make_unique<Piece>(COL_BLACK, PIECE_KING);
    initial_board_state.pieces[PIECE_ARRAY_BLACK_KING].position = SquarePosition{0, 0};

    initial_board_state.pieces[PIECE_ARRAY_WHITE_BISHOP_LEFT].piece =
        std::make_unique<Piece>(COL_WHITE, PIECE_BISHOP);
    initial_board_state.pieces[PIECE_ARRAY_WHITE_BISHOP_LEFT].position = SquarePosition{0, 1};

    initial_board_state.pieces_black = initial_board_state.pieces[PIECE_ARRAY_BLACK_KING].position.get_bitboard_mask();
    initial_board_state.pieces_white =
        initial_board_state.pieces[PIECE_ARRAY_WHITE_BISHOP_LEFT].position.get_bitboard_mask() |
            initial_board_state.pieces[PIECE_ARRAY_WHITE_KING].position.get_bitboard_mask();
    initial_board_state.pieces_kings =
        initial_board_state.pieces[PIECE_ARRAY_BLACK_KING].position.get_bitboard_mask() |
            initial_board_state.pieces[PIECE_ARRAY_WHITE_KING].position.get_bitboard_mask();
    initial_board_state.pieces_queens = 0;
    initial_board_state.pieces_rooks = 0;
    initial_board_state.pieces_bishops =
        initial_board_state.pieces[PIECE_ARRAY_WHITE_BISHOP_LEFT].position.get_bitboard_mask();
    initial_board_state.pieces_knights = 0;
    initial_board_state.pieces_pawns = 0;



    DecisionTree tree;
    tree.root = std::make_unique<DecisionTreeNode>(NodeData(initial_board_state));
    tree.root->data.board_state.print();

    std::cout << tree.root->children.size() << std::endl;

    tree.root->generate_moves();

    std::cout << tree.root->children.size() << std::endl;
    for (int i = 0; i < tree.root->children.size(); i++) {
        Move move = tree.root->children[i]->data.board_state.previous_move.value();
        std::cout << std::endl;
        move.print();
        std::cout << std::endl;
        tree.root->children[i]->data.board_state.print();
        tree.root->children[i]->check_legality();
        std::cout << (tree.root->children[i]->data.legality == NODE_LEGAL ? "LEGAL" : "ILLEGAL");
    }

    return 0;
}
