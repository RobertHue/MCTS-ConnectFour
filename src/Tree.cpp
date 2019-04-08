#include <string>
#include <iostream>
#include <queue>
#include <memory>
#include "Tree.h"
using namespace std;

Tree::Tree() : m_amountOfNodes(0) {
	cout << "constructor called " << endl;
    root = createNewNode();
}
Tree::~Tree() {
	cout << "destructor called " << endl;
	std::cout << "before call to deleteTree: " << m_amountOfNodes << "\n";
	deleteTree(root);
	std::cout << "after call to deleteTree: " << m_amountOfNodes << "\n";
}

Tree::Tree(const Tree &other)
{
	cout << "copy constructor called " << endl;
	// check for self-assignment
	if (this == &other) {
		return;
	}

	// assign the contents (deep copy)
	this->m_amountOfNodes = 0;
	this->root = copyTree(nullptr, other.root);
}

Tree & Tree::operator=(const Tree &other)
{
	cout << "copy assignment constructor called " << endl;

	// check for self-assignment
	if (this == &other) {
		return *this;
	}

	// assign the contents (deep copy)
	this->m_amountOfNodes = 0;
	this->root = copyTree(nullptr, other.root);

	return *this;
}

bool Tree::isEmpty() const {
    return !root;
}

NodeType * Tree::getRoot() {
    return root;
}

NodeType * Tree::createNewNode() {
    NodeType *newNode = new NodeType;

	newNode->UCTB = rand() % 1000 + 10000;	// assign unvisited nodes with a very large UCTB-value
	newNode->value = 0;
	newNode->visits = 0;
	newNode->parent = nullptr;
	++m_amountOfNodes;
    return newNode;
}

void Tree::addNodeTo(NodeType *newNode, NodeType *dstNode) {
	newNode->parent = dstNode;
	dstNode->childNodes.push_back(newNode);
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

int Tree::getAmountOfNodes() const
{
	return m_amountOfNodes;
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
	--m_amountOfNodes;
}

NodeType * Tree::copyTree(NodeType* parent, NodeType * other)
{
	if (other == nullptr)
	{
		return nullptr;
	}

	NodeType *newNode = createNewNode();
	newNode->chosenTurnThatLeadedToThisNode = other->chosenTurnThatLeadedToThisNode;
	newNode->parent = other->parent;
	newNode->sequenceThatLeadedToThisNode = other->sequenceThatLeadedToThisNode;
	newNode->UCTB = other->UCTB;
	newNode->value = other->value;
	newNode->visits = other->visits;
	newNode->parent = parent;

	newNode->childNodes = std::vector<NodeType *>(other->childNodes.size());
	/* first delete all subtrees (from left to right) */
	for (int i = 0; i < other->childNodes.size(); ++i) {
		newNode->childNodes[i] = copyTree(newNode, other->childNodes[i]);
	}
	 
	return newNode;
}

