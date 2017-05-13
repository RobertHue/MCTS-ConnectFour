#pragma once
#include <vector>


// data == gamepanel
//--------------------------
// Datenstrukturen fr MCTS
//--------------------------

typedef struct node t_node;

struct node {
    double UCTB; // = Bewertung nach UCT
    double value; // value[0,1] => bound nach UCT
    int visits; // visited => bound nach UCT
    t_node *parent; // Ursprung/Vater/Vorgnger 
    std::vector<t_node *> childNodes; // betrachtete Nachfolgerknoten
    int chosenTurnThatLeadedToThisNode; // aka the action
};

class Tree {
private:
    t_node *root;
public:
    Tree();

    bool is_empty() const;
    t_node * getRoot();
    t_node * createNewNode();

    static void addNodeTo(t_node *node, t_node *dst_node);

    static void printAllChildsUCTB(node *n);

    // Print the tree level-order assisted by queue
    static void levelOrder(node* n);
};