#ifndef TREE_H_
#define TREE_H_

#include <vector>
#include <string>

// forward declare NodeType so it can be used inside Node again
typedef struct Node		NodeType;

struct Node {
    double UCTB;            /// UCTB rating -- used for selection
	double winratio;        /// winratio -- used for exploitation
	double uct;				/// uct -- used for exploration
    double value;           /// accumulated reward value
    int visits;             /// total amount of visits

	/// the action/move that leaded to that node
	int chosenMoveThatLeadedToThisNode; 

	// the sequence of actions that leaded to that node (from root)
	std::string sequenceThatLeadedToThisNode;

    NodeType *parent;						/// parent
    std::vector<NodeType *> childNodes;     /// children
};


class Tree {
public:
    Tree();		/// constructor
	~Tree();	/// destructor
	Tree(const Tree& t2);	/// Copy constructor 
	Tree& operator = (const Tree &t3);	// copy assignment constructor

public:
    bool isEmpty() const;
    NodeType* getRoot();
	int getAmountOfNodes() const;

	// Prints the n-ary tree level wise (level-order)
	static void printLevelOrder(NodeType* rootNode);
	static void printChildNodeInfo(NodeType *n);

    NodeType* createNewNode();	// factory for new nodes
    static void addNodeTo(NodeType* newNode, NodeType* dstNode);

private:
	void deleteTree(NodeType* node);
	NodeType* copyTree(NodeType* parent, NodeType* other);

private:
    NodeType *root;
	int m_amountOfNodes;
};

#endif /* TREE_H_ */
