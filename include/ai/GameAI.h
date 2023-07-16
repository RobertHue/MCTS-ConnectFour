#pragma once

#include <cmath>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <time.h>

#include "ai/Tree.h"
#include "data/GameState.h"


/// @brief Used as the GameTree of the AI
typedef struct NodeData
{
    NodeData()
        : level(0), UCTB(0.0), winratio(0.0), uct(0.0), rating(0.0), visits(0),
          player(Player::NONE), chosenMoveThatLeadedToThisNode(-1),
          sequenceThatLeadedToThisNode("r")
    {
    }

    size_t level;       /// the level this nodes resides on in the game tree
    double UCTB;        /// UCTB rating -- used for selection
    double winratio;    /// winratio -- used for exploitation
    double uct;         /// uct -- used for exploration
    double rating;      /// accumulated reward rating
    std::size_t visits; /// total amount of visits also the simulation count
    Player
        player; /// stores the player that did the move leading to this node (used for the rating)


    int chosenMoveThatLeadedToThisNode; /// the action/move that leaded to that node


    std::string
        sequenceThatLeadedToThisNode; /// the sequence of actions that leaded to that node (from root)
} NodeDataType;

// aliases for easier access
using TreeType = Tree<NodeDataType>;
using NodeType = TreeType::NodeType;
using NodeTypePtr = TreeType::NodeTypePtr;

/// @brief ostream operator<< Overloading for NodeDataType
std::ostream &operator<<(std::ostream &os, const NodeDataType &nodeData);

void printChildNodeInfo(const NodeTypePtr node);

/// @brief this class is responsible for finding the next move for the AI in ConnectFour
/// @note It does this by applying the Monte Carlo Tree Search (MCTS).
/// There are 6 main parameters which can be change:
/// 	* MAX_NUM_OF_ITERATIONS
/// 	    * EXPAND_FULLY_ON_VISITS
/// 	    * Value::WIN
/// 	    * Value::DRAW
/// 	    * Value::LOOSE
/// 	    * CURIOUSITY_FACTOR
///
/// for more information on the mcts, see:
/// - http://de.slideshare.net/ftgaic/mcts-ai
/// - https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
/// - http://www.tantrix.com/Tantrix/TRobot/MCTS%20Final%20Report.pdf
class GameAI
{
public:
    const size_t MAX_NUM_OF_ITERATIONS = 10000; /// the number of iterations
    /// (each iteration resembles a simualation of one complete game)
    const size_t EXPAND_FULLY_ON_VISITS = 10; /// expands all nodes on a visit

    class Value
    {
    private:
        double f;

    public:
        Value() : f(0.0)
        {
        } // default constructor
        Value(double f) : f(f)
        {
        } // argument constructor
        operator double() const
        {
            return f;
        } // user-defined conversion (implicit)

        // for using Value x = Double(4.3);
        Value &operator=(double value)
        {
            f = value;
            return *this;
        }

    public:
        static constexpr double WIN = 1.0;
        static constexpr double DRAW = 0.5;
        static constexpr double LOOSE = 0.0;
    };

    /// @brief C - curiousity of the algorithm (empirically set to 1.41421L)
    /// @note Different meanings of the curiousity constant C:
    /// - constant C = CURIOUSITY_FACTOR = curiousity of the algorithm
    /// - small C => game tree gets deeper  expanded (only the best variation gets explored)
    /// - big C   => game tree gets broader expanded (nodes with lesser visits are prefered)
    const double CURIOUSITY_FACTOR = 1.41421L;

public:
    /// @brief Construct a GameAI for the given Player and let it take control over him
    ///        call calculateNextTurn to make the AI do the next move for the Player
    GameAI(Player tokenKI);

    /// @brief Finds the next move for the AI based on the MCTS-algorithm
    /// @return the MCTS-based move for the AI
    int findNextMove(const GameState &gameState);

    /// @brief Gets the game tree used for MCTS
    const TreeType &getGameTree() const;

    /// @brief Gets the ai player:
    Player getPlayer() const;

private:
    GameState simulatedGameState;
    Player AI_Player,
        OP_Player; // AI knows about opponent & user player token
    std::unique_ptr<TreeType> m_pGameTree; // the game tree

private:
    /// @brief expands all child nodes of passed node (mostly used for root (R))
    /// @note  also initializes those created child nodes with a
    ///        randomly assigned high UCTB-value
    void expandAllChildrenOf(NodeTypePtr nodeToExpand);

    /// @brief Selects the most promising child node and therefore the most promising move
    ///        until a leaf node L is reached.
    /// @note L is a node from which no simulation (playout) has yet been initiated.
    /// @note changes the simulatedGameState of this class
    NodeTypePtr selectPromisingNode(NodeTypePtr rootNode);

    NodeTypePtr selectMostVisitedChild(NodeTypePtr rootNode);

    /// @brief  Expands the give leaf_node
    /// @param leaf_node the leaf node
    /// @return	expanded node, otherwhise if it wasnt possible to expand the leaf_node L again
    /// @note first checks whether L ends the game in WIN/DRAW/LOOSE for either player
    ///       2nd if thats not the case, then create one/more child nodes and choose node C from one of them
    ///       child nodes are valid moves from the game position defined by leaf_node L
    NodeTypePtr expandNode(NodeTypePtr leaf_node);


    /// @brief Simulates a game from the expanded node (C) and
    ///        updates the game tree (just one simulation per call)
    /// @param expanded_node the expanded node
    /// @note the simulation is only needed to rate the expanded node(C) and
    ///       the path to that expanded node
    /// @return a double indicaing if the game was won by the AI
    double simulation(NodeTypePtr expanded_node);


    /// @brief Backpropagate/Update the ratings from the expanded node to its root node.
    /// @note use the result of the simulation (playout) to update (backpropagate)
    /// information in the nodes (of the game tree) on the path from
    /// expanded node (C) to root node (R)
    void backpropagation(NodeTypePtr expanded_node,
                         const Value ratingToBeUpdated);

private:
    /// selects the most visited child node from the game tree rootNode
    NodeTypePtr selectMostVisitedChild(NodeTypePtr rootNode);
    NodeTypePtr findBestNodeWithUCT(NodeTypePtr node);
    double uctValue(NodeTypePtr node);

    int pickBestMoveFrom(std::vector<int> &possibleMoves, const GameState &gs);

    int pickRandomMoveFrom(std::vector<int> &possibleMoves,
                           const GameState &gs);

    /// @brief Picks the best move it can do for the next turn (greedy)
    /// @note If not possible, it will pick a random placement:
    ///
    ///  @Implementation
    /// 		based on TRIAL AND ERROR:
    ///  test each column whether a move will lead to a win for the turnPlayer
    ///  (doesn't matter whether KI or human player)
    ///  it needs to simulate turns so that counts for the simulated
    /// turn of opponent too (the KI predicts what his opponent will be doing)
    ///  If no winning turn is possible or
    ///  the avoidance of opponent winning is not possible with this move,
    ///  then select a random move
    ///
    /// @note This function does not change the passed GameState-object (const)
    /// @return The column col (0-7), otherwhise -1 when every column is already full
    int pickBestMove(const GameState &gp);

    /// @brief Picks a random move in column
    /// @return The column col (0-7), otherwhise -1 when every column is already full
    int pickRandomMove(const GameState &gp);

    /// @brief Picks the best child (aka the child with the best promising move)
    NodeTypePtr pickBestChild(NodeTypePtr node, const GameState &gs);

    /// @brief Picks a random child
    NodeTypePtr pickRandomChild(NodeTypePtr node);

    /// @brief Do a random move
    /// @return The move (aka the chosen column)
    /// @note Changes the passed GameState
    int doRandomMove(GameState &gp);

    std::vector<int> setupPossibleMovesOf(NodeTypePtr node);
};
