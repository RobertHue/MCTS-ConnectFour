#ifndef TREE_H_
#define TREE_H_


#include <cstddef>  // for std::size_t
#include <iomanip>  // for std::setw(n)
#include <iostream> // for std::cout
#include <memory>   // for std::shared_ptr
#include <queue>    // for std::queue  (level-order-traversal)
#include <string>   // for std::string
#include <vector>   // for std::vector (childNodes)


/////////////////////////
//////// N o d e ////////
/////////////////////////


template <typename T>
class Node
{
public:
    using NodeType = std::shared_ptr<Node<T>>;
    using ChildNodeType = std::vector<NodeType>;

public:
    Node();

    // Rule of Five-idiom
    ~Node() noexcept = default;                    // destructor
    Node(const Node<T> &src) = default;            // copy construct
    Node &operator=(const Node<T> &rhs) = default; // copy assignment construct
    Node(Node<T> &&src) noexcept = default;        // move construct
    Node &operator=(Node<T> &&rhs) noexcept = default; // move assign construct

    NodeType parent;                                   /// parent
    ChildNodeType childNodes;                          /// children
    T data;                                            /// data
};


template <typename T>
inline Node<T>::Node() : parent(nullptr), childNodes(), data()
{
}


/////////////////////////
//////// T r e e ////////
/////////////////////////


template <typename T>
class Tree
{
public:
    using NodeType = Node<T>;
    using NodeTypePtr = std::shared_ptr<NodeType>;

public:
    Tree();

    // Rule of Five-idiom
    ~Tree() noexcept = default;                    // destructor
    Tree(const Tree<T> &src) = default;            // copy construct
    Tree &operator=(const Tree<T> &rhs) = default; // copy assignment construct
    Tree(Tree<T> &&src) noexcept = default;        // move construct
    Tree &operator=(Tree<T> &&rhs) noexcept = default; // move assign construct

public:
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] NodeTypePtr getRoot() const;
    [[nodiscard]] std::size_t size() const; // returns the number of nodes

    // prints the level order from the position of the root node
    //enables prettier calls, eg: tree.printLevelOrder();
    void printLevelOrder();

    // creates a new node at destination with default values returning the newly created node
    [[maybe_unused]] NodeTypePtr createNewNode(NodeTypePtr dstNode);

public: /* STATIC */
    // Prints the n-ary tree level wise (level-order)
    static void printLevelOrder(NodeTypePtr rootNode);

private:
    // clears the subtree from the position of the passed node, used by destructor
    void clear(NodeTypePtr node);

    // copies the tree from the right to the left, used by copy constructors
    [[nodiscard]] NodeTypePtr copyTree(NodeTypePtr parent, NodeTypePtr other);

private:
    NodeTypePtr m_root; // stores a pointer to the root node of the tree
    int m_amountOfNodes{}; // cached value of the tree size, so no traversal needed anymore to find it out
};


template <typename T>
Tree<T>::Tree() : m_root(createNewNode(nullptr))
{
}


template <typename T>
std::ostream &operator<<(std::ostream &, const Node<T> &node);


////////////////////////////////////////////////////////////////////////////////


template <typename T>
inline bool Tree<T>::isEmpty() const
{
    return !m_root;
}


template <typename T>
inline typename Tree<T>::NodeTypePtr Tree<T>::getRoot() const
{
    return m_root;
}


template <typename T>
inline typename Tree<T>::NodeTypePtr Tree<T>::createNewNode(NodeTypePtr dstNode)
{
    NodeTypePtr newNode = std::make_shared<NodeType>();

    // addNodeTo destination node
    newNode->parent = dstNode;
    if (dstNode != nullptr)
    {
        dstNode->childNodes.push_back(newNode);
    }

    ++m_amountOfNodes;
    return newNode;
}


template <typename T>
inline void Tree<T>::printLevelOrder()
{
    Tree<T>::printLevelOrder(m_root);
}


template <typename T>
inline std::size_t Tree<T>::size() const
{
    return m_amountOfNodes;
}


// Print the tree level-order assisted by queue
template <typename T>
inline void Tree<T>::printLevelOrder(NodeTypePtr rootNode)
{
    if (rootNode == nullptr)
    {
        return;
    }
    std::queue<NodeTypePtr> q; // Create a queue (FIFO)
    q.push(rootNode);          // Enqueue root

    int level = 0;
    while (!q.empty())
    {
        std::cout << "\n" << level++ << ": \t";
        int n = q.size();

        // If this node has children
        while (n > 0)
        {
            // Dequeue an item from queue and print it
            NodeTypePtr p = q.front();
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


template <typename T>
inline void Tree<T>::clear(NodeTypePtr node)
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

    --m_amountOfNodes;
}


template <typename T>
inline typename Tree<T>::NodeTypePtr Tree<T>::copyTree(NodeTypePtr parent,
                                                       NodeTypePtr src)
{
    if (src == nullptr)
    {
        return nullptr;
    }

    // create newNode and copy data
    NodeTypePtr newNode = createNewNode(parent);
    newNode->data = src->data;

    // copy all subtrees from current node (parent)
    for (std::size_t i = 0; i < src->childNodes.size(); ++i)
    {
        newNode->childNodes[i] = copyTree(newNode, src->childNodes[i]);
    }

    // return currently copied
    return newNode;
}


template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Node<T> &node)
{
    os << node.data;
    return os; // enables concatenation of ostreams with <<
}


#endif /* TREE_H_ */
