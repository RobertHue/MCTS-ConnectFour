#include <iostream>
#include <iomanip>
#include <string>

#include <queue>
#include <memory>

#include "Tree.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////

Tree::Tree() : m_amountOfNodes(0) {
	//cout << "constructor called " << endl;
    root = createNewNode();
}
Tree::~Tree() {
	//cout << "destructor called " << endl;
	//std::cout << "before call to deleteTree: " << m_amountOfNodes << "\n";
	deleteTree(root);
	//std::cout << "after call to deleteTree: " << m_amountOfNodes << "\n";
}

Tree::Tree(const Tree &other)
{
	//cout << "copy constructor called " << endl;
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
	//cout << "copy assignment constructor called " << endl;

	// check for self-assignment
	if (this == &other) {
		return *this;
	}

	// assign the contents (deep copy)
	this->m_amountOfNodes = 0;
	this->root = copyTree(nullptr, other.root);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////

bool Tree::isEmpty() const {
    return !root;
}

NodeType * Tree::getRoot() {
    return root;
}

NodeType * Tree::createNewNode() {
    NodeType *newNode = new NodeType;

	newNode->UCTB = rand() % 1000 + 10000;	// assign unvisited nodes with a very large UCTB-rating
	newNode->winratio = 0.0;
	newNode->uct = 0.0;
	newNode->rating = 0;
	newNode->visits = 0;
	newNode->parent = nullptr;
	++m_amountOfNodes;
    return newNode;
}

void Tree::addNodeTo(NodeType *newNode, NodeType *dstNode) {
	newNode->parent = dstNode;
	dstNode->childNodes.push_back(newNode);
}


// Print the tree level-order assisted by queue

void Tree::printLevelOrder(NodeType* rootNode) {
	if (rootNode == nullptr) { return; }
    queue<NodeType *> q; // Create a queue (FIFO)
	q.push(rootNode); // Enqueue root  

    int level = 0;
    while (!q.empty()) 
	{
		cout << "\n" << level++ << ": \t";
		int n = q.size();

		// If this node has children 
		while (n > 0)
		{
			// Dequeue an item from queue and print it 
			NodeType * p = q.front();
			q.pop();
			cout << setw(10) << p->chosenMoveThatLeadedToThisNode << "|" << p->rating << " / " << p->visits << " ";

			// Enqueue all children of the dequeued item 
			for (int i = 0; i < p->childNodes.size(); i++)
				q.push(p->childNodes[i]);
			n--;
		}

		cout << endl; // Print new line between two levels
    }
    cout << endl << endl;
}


void Tree::printChildNodeInfo(NodeType *n) {
	cout << "-----------------------\n";
	cout << "Child Node Info:\n";
	NodeType* tmpNode = n;
	int firstTab = 15;
	int secondTab = 8;
	for (auto& n : tmpNode->childNodes) {

		cout << "[ Move:"
			<< n->chosenMoveThatLeadedToThisNode
			<< setw(firstTab) << "Wins:"		<< setw(secondTab) << n->rating
			<< setw(firstTab) << "Visits:"		<< setw(secondTab) << n->visits
			<< setw(firstTab) << "UCTB:"		<< setw(secondTab) << n->UCTB
			<< setw(firstTab) << "winratio:"	<< setw(secondTab) << n->winratio
			<< setw(firstTab) << "uct:"			<< setw(secondTab) << n->uct
			<< "]\n";
	}
	cout << "-----------------------\n";

	/* was former:
	cout << "GameTree:" << endl;
	for (size_t i = 0; i < n->childNodes.size(); ++i) {
		cout << n->childNodes[i]->UCTB << "  ";
	}
	cout << endl;
	*/
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
	//cout << "\n\n Deleting node: " << node->UCTB << "\n\n";
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
	newNode->chosenMoveThatLeadedToThisNode = other->chosenMoveThatLeadedToThisNode;
	newNode->parent = other->parent;
	newNode->sequenceThatLeadedToThisNode = other->sequenceThatLeadedToThisNode;
	newNode->UCTB = other->UCTB;
	newNode->rating = other->rating;
	newNode->visits = other->visits;
	newNode->parent = parent;

	newNode->childNodes = std::vector<NodeType *>(other->childNodes.size());
	/* first delete all subtrees (from left to right) */
	for (int i = 0; i < other->childNodes.size(); ++i) {
		newNode->childNodes[i] = copyTree(newNode, other->childNodes[i]);
	}
	 
	return newNode;
}

