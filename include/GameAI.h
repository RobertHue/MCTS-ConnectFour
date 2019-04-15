#pragma once

#include <queue>
#include <stack>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "GameState.h"
#include "Tree.h"

// nodedata is used for the gametree
//struct NodeData;	// struct declaration (can be used to forward declare)
typedef struct NodeData {	// struct definition
	NodeData() : 
		level(0), 
		UCTB(0.0),
		winratio(0.0), 
		uct(0.0), 
		rating(0.0), 
		visits(0), 
		player(Player::NONE), 
		chosenMoveThatLeadedToThisNode(-1), 
		sequenceThatLeadedToThisNode("root") 
	{}

	size_t	level;		/// the level this nodes resides on in the game tree
	double	UCTB;       /// UCTB rating -- used for selection
	double	winratio;   /// winratio -- used for exploitation
	double	uct;		/// uct -- used for exploration
	double	rating;     /// accumulated reward rating
	int		visits;     /// total amount of visits also the simulation count
	Player	player;		/// stores the player that did the move leading to this node (used for the rating)

	/// the action/move that leaded to that node
	int chosenMoveThatLeadedToThisNode; // todo LEADED => LEAD

	// the sequence of actions that leaded to that node (from root)
	std::string sequenceThatLeadedToThisNode;
} NodeDataType;

std::ostream& operator<<(std::ostream& os, const NodeDataType& node);

using TreeType = Tree<NodeDataType>;
using NodeType = TreeType::NodeType;

void printChildNodeInfo(const NodeType * node);


 
/**
@brief	this class is responsible for finding the next move for the AI in ConnectFour
		It does this by applying the Monte Carlo Tree Search (MCTS).

		There are 6 main parameters which can be change:

		* MAX_NUM_OF_ITERATIONS
		* EXPAND_FULLY_ON_VISITS
		* Value::WIN
		* Value::DRAW
		* Value::LOOSE
		* CURIOUSITY_FACTOR
		
	for more information on the mcts, see:
	http://de.slideshare.net/ftgaic/mcts-ai
	https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
	http://www.tantrix.com/Tantrix/TRobot/MCTS%20Final%20Report.pdf
*/
class GameAI {
public:
	/// the number of iterations(each iteration resembles a simualation of one complete game)
	const size_t MAX_NUM_OF_ITERATIONS = 10000;
	const size_t EXPAND_FULLY_ON_VISITS = 10; /// @todo currently not used because it is not passing the tests

	class Value {
	private:
		double f;
	public:
		Value() : f(0.0) {}						// default constructor
		Value(double f) : f(f) {}				// argument constructor
		operator double() const { return f; }	// user-defined conversion (implicit)
	public:
		static constexpr double WIN		= 1.0;
		static constexpr double DRAW	= 0.5;
		static constexpr double LOOSE	= 0.0;
	};
	/**	
		C - curiousity of the algorithm (empirically set to 1.41421L)

		constant C = CURIOUSITY_FACTOR = curiousity of the algorithm
		small C => game tree gets deeper  expanded (only the best variation gets explored)
		big C   => game tree gets broader expanded (nodes with lesser visits are prefered)
	*/
	const double CURIOUSITY_FACTOR = 1.41421L;
public:

	/// construct a GameAI for the given Player and let it take control over him
	/// call calculateNextTurn to make the AI do the next move for the Player
    GameAI(Player tokenKI);

	/// calculates the next move for the AI based on the MCTS-algorithm
	/// @return		the MCTS-based move for the AI
    int findNextMove(const GameState &gPanel);

	/// gets the game tree used for MCTS
	TreeType* getGameTree() const;

private:
    // const GameState &actualGameState;
    GameState simulatedGameState;
    Player AI_Player, OP_Player; // AI knows about opponent & user player token
    std::unique_ptr<TreeType> m_pGameTree;	// the game tree
	////////////////////////////////////////////////////////////////////////

private:
	/// expands all child nodes of passed node (mostly used for root (R)) 
	/// also initializes those created child nodes with a 
	/// randomly assigned high UCTB-value
    void expandAllChildrenOf(NodeType *Node);
	
	/// Selects the most promising child node and therefore the most promising move
	/// until a leaf node L is reached.
	/// @note	L is a node from which no simulation (playout) has yet been initiated.
	/// @note	changes the simulatedGameState of this class
	NodeType *selectPromisingNode(NodeType *rootNode);

	/// expands the give leaf_node
	/// @return	expanded node, otherwhise if it wasnt possible to expand the leaf_node L again
	/// @note	first checks whether L ends the game in WIN/DRAW/LOOSE for either player
	///			2nd if thats not the case, then create one/more child nodes and choose node C from one of them
	///			child nodes are valid moves from the game position defined by leaf_node L
    NodeType *expandNode(NodeType *leaf_node);


	/**
	 @brief		simulates a game from the expanded node (C) and 
				updates the game tree (just one simulation per call)

	 @note		the simulation is only needed to rate the expanded node (C) and 
				the path to that expanded node

	 @return	a double indicaing if the game was won by the AI
	 */
    double simulation(NodeType *expanded_node);


	/// use the result of the simulation (playout) to update (backpropagate) information in 
	/// the nodes (of the game tree) on the path from 
	/// expanded node (C) to root node (R)
    void backpropagation(NodeType *expanded_node, const Value ratingToBeUpdated);


	///////////////////////////////////////////////////////////////////////////

private:
	/// selects the most visited child node from the game tree rootNode
    NodeType *selectMostVisitedChild(const NodeType *rootNode);
	NodeType *findBestNodeWithUCT(NodeType *node);
	double uctValue(NodeType* node);

	int pickBestMoveFrom(std::vector<int>& possibleMoves, const GameState & gs);

	int pickRandomMoveFrom(std::vector<int>& possibleMoves, const GameState & gs);

	////////////////////////////////////////////////////////////////////////////
	/// highly coupled methods depending on the kind of game (here connect four)
	////////////////////////////////////////////////////////////////////////////
	/**
	 * @brief	tries to pick the best move it can do for the next turn (greedy)
	 *			if not possible, it will pick a random placement
	 *
	 * @Implementation 
	 *		based on TRIAL AND ERROR:
	 * test each column whether a move will lead to a win for the turnPlayer 
	 * (doesn't matter whether KI or human player)
	 * it needs to simulate turns so that counts for the simulated 
	 * turn of opponent too (the KI predicts what his opponent will be doing)
	 * If no winning turn is possible or 
	 * the avoidance of opponent winning is not possible with this move,
	 * then select a random move
	 *
	 * @Note: this function does not change the passed GameState-object (const)
	 */
    int pickBestMove(const GameState &gp);

	/**
	 * @brief:	picks a random move in column
	 * @return: the column col (0-7)
	 *		returns -1 when every column is already full
	 */
    int pickRandomMove(const GameState &gp);

	NodeType * pickBestChild(NodeType * node, const GameState & gs);

	NodeType * pickRandomChild(NodeType * node);

	// macht einen zuflligen spielzug und ndert gameDataH
	// es wird die position des Spielsteins zurckgegeben
	/**
	* @brief	do a random move
	* @return	the move (aka the chosen column)
	* @note		changes the passed GameState
	*/
    int doRandomMove(GameState &gp);

	std::vector<int> setupPossibleMovesOf(NodeType *node);
};