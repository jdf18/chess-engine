#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <vector>
#include <memory>

#include "boardstate.h"

typedef struct NodeData {
    BoardState board_state;
    Move previous_move;

    NodeData(NodeData const &copy) = default;
    NodeData(BoardState const &board_state, Move const &previous_move) : board_state(board_state), previous_move(previous_move) {};
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

    void generate_moves();

private:
    void add_child(NodeData child_data);

} DecisionTreeNode;

typedef class DecisionTree {
public:
    DecisionTreeNode root;
} DecisionTree;

#endif //DECISIONTREE_H
