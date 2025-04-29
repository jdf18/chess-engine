#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <vector>
#include <memory>

#include "boardstate.h"

typedef struct NodeData {
    BoardState board_state;

    NodeData(NodeData const &copy) = default;
    explicit NodeData(BoardState const &board_state) : board_state(board_state) {};

    bool initialise_node(); // Returns if the move was valid or not

    float evaluate();

    float material_sum(Colour col);
} NodeData;

typedef class DecisionTreeNode {
public:
    NodeData data;
    std::vector<std::unique_ptr<DecisionTreeNode>> children;

    DecisionTreeNode(const DecisionTreeNode& copy)
        : data(copy.data) {
        for (const auto &child : copy.children) {
            children.push_back(std::make_unique<DecisionTreeNode>(*child));
        }
    };

    explicit DecisionTreeNode(const NodeData& node_data) : data(node_data) {};

    void generate_castle_moves();
    void generate_en_passant_moves();
    void generate_moves();

    MoveEvaluated return_best_move(uint8_t depth, Colour player);

private:
    void add_child(NodeData child_data);

} DecisionTreeNode;

typedef class DecisionTree {
public:
    std::unique_ptr<DecisionTreeNode> root;
} DecisionTree;

#endif //DECISIONTREE_H
