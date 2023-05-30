#ifndef TREE_H_
#define TREE_H_

#include <cstddef>  // for std::size_t
#include <iomanip>  // for std::setw(n)
#include <iostream> // for std::cout
#include <queue>    // for std::queue  (level-order-traversal)
#include <string>   // for std::string
#include <vector>   // for std::vector (childNodes)

/////////////////////////
//////// N o d e ////////
/////////////////////////

template <typename _DataType>
class Node
{
public:
    Node();
    Node(const _DataType &src);
    Node(_DataType &&src);

    Node<_DataType> *parent;                   /// parent
    std::vector<Node<_DataType> *> childNodes; /// children
    _DataType data;                            /// data
};

//////////////////////////////////////////////////

template <typename _DataType>
inline Node<_DataType>::Node() : parent(nullptr), childNodes(), data()
{
}

template <typename _DataType>
inline Node<_DataType>::Node(const _DataType &src)
    : parent(nullptr), childNodes(), data(src)
{
}

template <typename _DataType>
inline Node<_DataType>::Node(_DataType &&src)
    : parent(nullptr), childNodes(), data(src)
{
}

/////////////////////////
//////// T r e e ////////
/////////////////////////

template <typename _DataType>
class Tree
{
public:
    using NodeType = Node<_DataType>; /// alias type for the template type used

public:
    Tree();                                      /// constructor
    ~Tree();                                     /// destructor
    Tree(const Tree<_DataType> &src);            /// Copy constructor
    Tree &operator=(const Tree<_DataType> &rhs); // copy assignment constructor
    // destructor, copy constructor and copy assignment constructor
    // just needed for when pointer is used

public:
    bool isEmpty() const;
    Node<_DataType> *getRoot() const;
    std::size_t size() const; /// returns the number of nodes

    // Prints the n-ary tree level wise (level-order)
    static void printLevelOrder(NodeType *rootNode);
    void
    printLevelOrder(); // prints the level order from the position of the root node
    //enables prettier calls, eg: tree.printLevelOrder();

    NodeType *createNewNode(
        NodeType *
            dstNode); /// creates a new node at destination with default values returning the newly created node

private:
    void clear(
        NodeType *
            node); // clears the subtree from the position of the passed node, used by destructor
    NodeType *copyTree(
        NodeType *parent,
        NodeType *
            other); // copies the tree on the right to the left, used by copy constructors

private:
    NodeType *m_root; // stores a pointer to the root node of the tree
    int m_amountOfNodes; // cached value of the tree size, so no traversal needed anymore to find it out
};

template <typename _DataType>
std::ostream &operator<<(
    std::ostream &,
    const Node<_DataType> &
        node); // friend to ostream not needed because it has public access to struct anyways

//////////////////////////////////////////////////

template <typename _DataType>
Tree<_DataType>::Tree() : m_amountOfNodes(0)
{
    //cout << "constructor called \n";
    m_root = createNewNode(nullptr);
}

template <typename _DataType>
Tree<_DataType>::~Tree()
{
    //cout << "destructor called \n";
    //std::cout << "before call to clear: " << m_amountOfNodes << "\n";
    clear(m_root);
    //std::cout << "after call to clear: " << m_amountOfNodes << "\n";
}

template <typename _DataType>
Tree<_DataType>::Tree(const Tree<_DataType> &src)
{
    //cout << "copy constructor called \n";
    // check for self-assignment
    if (this == &src)
    {
        return;
    }

    // assign the contents (deep copy)
    m_amountOfNodes = 0;
    m_root = copyTree(nullptr, src.m_root);
}

template <typename _DataType>
Tree<_DataType> &Tree<_DataType>::operator=(const Tree<_DataType> &rhs)
{
    //cout << "copy assignment constructor called \n";

    // check for self-assignment
    if (this == &rhs)
    {
        return *this;
    }

    // assign the contents (deep copy)
    this->m_amountOfNodes = 0;
    this->m_root = copyTree(nullptr, rhs.m_root);

    return *this;
}

////////////////////////////////////////////////////////////////////////////////

template <typename _DataType>
inline bool Tree<_DataType>::isEmpty() const
{
    return !m_root;
}

template <typename _DataType>
inline Node<_DataType> *Tree<_DataType>::getRoot() const
{
    return m_root;
}

template <typename _DataType>
inline Node<_DataType> *Tree<_DataType>::createNewNode(NodeType *dstNode)
{
    NodeType *newNode = new NodeType;

    // addNodeTo destination node
    newNode->parent = dstNode;
    if (dstNode != nullptr)
    {
        dstNode->childNodes.push_back(newNode);
    }

    ++m_amountOfNodes;
    return newNode;
}

// Print the tree level-order assisted by queue
template <typename _DataType>
inline void Tree<_DataType>::printLevelOrder(NodeType *rootNode)
{
    if (rootNode == nullptr)
    {
        return;
    }
    std::queue<NodeType *> q; // Create a queue (FIFO)
    q.push(rootNode);         // Enqueue root

    int level = 0;
    while (!q.empty())
    {
        std::cout << "\n" << level++ << ": \t";
        int n = q.size();

        // If this node has children
        while (n > 0)
        {
            // Dequeue an item from queue and print it
            NodeType *p = q.front();
            q.pop();
            std::cout << std::setw(10) << *p << " ";

            // Enqueue all children of the dequeued item
            for (std::size_t i = 0; i < p->childNodes.size(); i++)
            {
                q.push(p->childNodes[i]);
            }
            n--;
        }

        std::cout << "\n"; // Print new line between two levels
    }
    std::cout << "\n\n";
}

template <typename _DataType>
inline void Tree<_DataType>::printLevelOrder()
{
    Tree<_DataType>::printLevelOrder(m_root);
}

template <typename _DataType>
inline std::size_t Tree<_DataType>::size() const
{
    return m_amountOfNodes;
}

template <typename _DataType>
inline void Tree<_DataType>::clear(NodeType *node)
{
    if (node == nullptr)
    {
        return;
    }

    /* first delete all subtrees (from left to right) */
    for (std::size_t i = 0; i < node->childNodes.size(); ++i)
    {
        clear(node->childNodes[i]);
    }

    /* then delete the node */
    //cout << "\n\n Deleting node: " << node->UCTB << "\n\n";
    delete node;
    --m_amountOfNodes;
}

template <typename _DataType>
inline Node<_DataType> *Tree<_DataType>::copyTree(NodeType *parent,
                                                  NodeType *src)
{
    if (src == nullptr)
    {
        return nullptr;
    }

    // create newNode and copy data
    NodeType *newNode = createNewNode(parent);
    newNode->data = src->data;

    // copy all subtrees from current node (parent)
    for (std::size_t i = 0; i < src->childNodes.size(); ++i)
    {
        newNode->childNodes[i] = copyTree(newNode, src->childNodes[i]);
    }

    // return currently copied
    return newNode;
}

template <typename _DataType>
inline std::ostream &operator<<(std::ostream &os, const Node<_DataType> &node)
{
    os << node.data;
    return os; // enables concatenation of ostreams with <<
}

#endif /* TREE_H_ */
