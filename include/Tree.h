#ifndef TREE_H_
#define TREE_H_

#include <vector>
#include <string>


// data == gamepanel
//--------------------------
// Datenstrukturen fr MCTS
//--------------------------

// forward declare NodeType so it can be used inside Node again
typedef struct Node NodeType;
typedef struct PtNode NodeTypePT;

struct Node {
    double UCTB;            /// UCTB rating
    double value;           /// accumulated reward value
    int visits;             /// total amount of visits
	int chosenTurnThatLeadedToThisNode;     /// the action/move that leaded to that node
	std::string sequenceThatLeadedToThisNode;	// the sequence of actions that leaded to that node (from root)

    NodeType *parent;       /// parent
    std::vector<NodeType *> childNodes;     /// children
};

struct PtNode {
	double UCTB;            /// UCTB rating
	double value;           /// accumulated reward value
	int visits;             /// total amount of visits
	int chosenTurnThatLeadedToThisNode;     /// the action/move that leaded to that node
};

class Tree {
public:
    Tree();
	~Tree();
	Tree(const Tree& t2);	/// Copy constructor 
	Tree& operator = (const Tree &t3);	// copy assignment constructor

    bool isEmpty() const;
    NodeType* getRoot();
    NodeType* createNewNode();

    static void addNodeTo(NodeType* newNode, NodeType* dstNode);
    static void printAllChildsUCTB(Node *n);

    // Print the tree level-order assisted by queue
    static void levelOrder(Node* n);

	int getAmountOfNodes() const;

private:
	void deleteTree(NodeType* node);
	NodeType* copyTree(NodeType* parent, NodeType* other);
private:
    NodeType *root;
	int m_amountOfNodes;
};

#endif /* TREE_H_ */
