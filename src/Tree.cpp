#include <string>
#include <iostream>
#include <queue>
#include "Tree.h"
using namespace std;

Tree::Tree() {
    root = createNewNode();
}

bool Tree::isEmpty() const {
    return !root;
}

NodeType * Tree::getRoot() {
    return root;
}

NodeType * Tree::createNewNode() {
    NodeType *Node = new NodeType;

    Node->UCTB = 0;
    Node->value = 0;
    Node->visits = 0;
    Node->parent = NULL;

    return Node;
}

void Tree::addNodeTo(NodeType *Node, NodeType *dst_node) {
    Node->parent = dst_node;
    dst_node->childNodes.push_back(Node);
}

void Tree::printAllChildsUCTB(Node *n) {
    cout << "GameTree:" << endl;
    for (size_t i = 0; i < n->childNodes.size(); ++i) {
        cout << n->childNodes[i]->UCTB << "  ";

    }
    cout << endl;
}

// Print the tree level-order assisted by queue

void Tree::levelOrder(Node* n) {
    // Create a queue
    queue<NodeType *> q;

    // Push all the root's neighboors
    for (size_t i = 0; i < n->childNodes.size(); ++i) {
        q.push((n->childNodes[i]));
    }

    int countPulled = 0;
    int level = 0;
    cout << "\n" << level << ": ";

    while (!q.empty()) {
        // Dequeue a node from front
        Node* v = q.front();
        ++countPulled;
        cout << v->UCTB << " ";

        for (size_t i = 0; i < v->childNodes.size(); ++i) {
            q.push((v->childNodes[i]));
        }

        // Pop the visited node
        q.pop();
        if(countPulled%8==0) {
            cout << "\n" << ++level << ": ";
        }
    }
    cout << endl << endl;
}
