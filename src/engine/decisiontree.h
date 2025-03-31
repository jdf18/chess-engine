#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <vector>
#include <memory>

#include "boardstate.h"

typedef struct NodeData {
    BoardState board_state;
    Move previous_move;
} NodeData;

typedef class DecisionTreeNode {
public:
    NodeData data;
    std::vector<std::unique_ptr<DecisionTreeNode>> children;

    DecisionTreeNode(NodeData data) : data(data) {};
    // Dont need a destructor here as using unique pointers to child states
    //   If the unique pointer is deleted, all its children are recursively deleted too

    void generate_moves();

private:
    void add_child(NodeData child_data);

} DecisionTreeNode;

typedef class DecisionTree {
public:
    DecisionTreeNode root;
} DecisionTree;

#endif //DECISIONTREE_H
