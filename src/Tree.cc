#include <string>
#include <iostream>
#include <queue>
#include "Tree.h"
using namespace std;

Tree::Tree() {
    root = createNewNode();
}

bool Tree::is_empty() const {
    return !root;
}

t_node * Tree::getRoot() {
    return root;
}

t_node * Tree::createNewNode() {
    t_node *node = new t_node;

    node->UCTB = 0;
    node->value = 0;
    node->visits = 0;
    node->parent = NULL;

    return node;
}

void Tree::addNodeTo(t_node *node, t_node *dst_node) {
    node->parent = dst_node;
    dst_node->childNodes.push_back(node);
}

void Tree::printAllChildsUCTB(node *n) {
    int i = 0;
    for (int i = 0; i < n->childNodes.size(); ++i) {
        cout << n->childNodes[i]->UCTB << "  ";

    }
    cout << endl;
}

// Print the tree level-order assisted by queue

void Tree::levelOrder(node* n) {
    // Create a queue
    queue<t_node *> q;

    // Push the root and its neighboors
    q.push(n);

    int level = 0;
    cout << level << ": ";

    while (!q.empty()) {
        // Dequeue a node from front
        node* v = q.front();
        cout << v->UCTB << " ";

        for (int i = 0; i < v->childNodes.size(); ++i) {
            q.push((v->childNodes[i]));
        }

        // Pop the visited node 
        q.pop();
    }
    cout << endl << endl;
}
