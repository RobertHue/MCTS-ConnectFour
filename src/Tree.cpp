#include <string>
#include <iostream>
#include <queue>
#include "Tree.h"
using namespace std;

Tree::Tree() {
    root = createNewNode();
}
Tree::~Tree() {
	deleteTree(root);
}

bool Tree::isEmpty() const {
    return !root;
}

NodeType * Tree::getRoot() {
    return root;
}

NodeType * Tree::createNewNode() {
    NodeType *Node = new NodeType;

    Node->UCTB = rand() % 1000 + 10000;	// assign unvisited nodes with a very large UCTB-value
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
	if (n == nullptr) { return; }

    // Create a queue (FIFO)
    queue<NodeType *> q;
	
	// push root to the queue
	q.push(n);

    int countNeededForNextLevel = 1;
    int level = 0;
	cout << "\n" << level++ << ": ";
    while (!q.empty()) {
		// Dequeue a node from front
		Node* v = q.front();
		cout << v->UCTB << " ";
		--countNeededForNextLevel;
		if (countNeededForNextLevel == 0) {
			cout << "\n" << level++ << ": ";
			countNeededForNextLevel = v->childNodes.size();
		}

		// enqueue v's childrens (first left then right) to q
        for (size_t i = 0; i < v->childNodes.size(); ++i) {
            q.push((v->childNodes[i]));
        }
		
        // Pop the visited node
        q.pop();
    }
    cout << endl << endl;
}

void Tree::deleteTree(NodeType * node)
{
	if (node == nullptr) { return; }

	/* first delete all subtrees (from left to right) */
	for (int i = 0; i < node->childNodes.size(); ++i) {
		deleteTree(node->childNodes[i]);
	} 

	/* then delete the node */
	cout << "\n\n Deleting node: " << node->UCTB << "\n\n";
	delete node;
}
