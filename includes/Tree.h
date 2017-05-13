#ifndef TREE_H_
#define TREE_H_

#include <vector>


// data == gamepanel
//--------------------------
// Datenstrukturen fr MCTS
//--------------------------

// forward declare NodeType so it can be used inside Node again
typedef struct Node NodeType;

struct Node {
    double UCTB;            // = Bewertung nach UCT
    double value;           // value[0,1] => bound nach UCT
    int visits;             // visited => bound nach UCT
    NodeType *parent;       // Ursprung/Vater/Vorgnger
    std::vector<NodeType *> childNodes;     // betrachtete Nachfolgerknoten
    int chosenTurnThatLeadedToThisNode;     // aka the action
};

class Tree {
public:
    Tree();

    bool isEmpty() const;
    NodeType* getRoot();
    NodeType* createNewNode();

    static void addNodeTo(NodeType* Node, NodeType* dst_node);
    static void printAllChildsUCTB(Node *n);

    // Print the tree level-order assisted by queue
    static void levelOrder(Node* n);

private:
    NodeType *root;
};

#endif /* TREE_H_ */
