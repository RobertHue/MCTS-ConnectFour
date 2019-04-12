#pragma once

#include <queue>
#include <stack>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "GameState.h"
#include "Tree.h"


/// @TODO more description here
/// for more information on the mcts, see:
///		http://de.slideshare.net/ftgaic/mcts-ai
///		https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
class GameAI {
public:
	const size_t MAX_NUM_OF_ITERATIONS = 10000;
	const size_t T = 10;
	// the number of iterations 
	// (each iteration resembles a simualation of one complete game)
public:

	/// construct a GameAI for the given Player and let it take control over him
	/// call calculateNextTurn to make the AI do the next move for the Player
    GameAI(Player tokenKI);

	/// calculates the next move for the AI based on the MCTS-algorithm
	/// @return		the MCTS-based move for the AI
    int findNextMove(const GameState &gPanel);

	/// gets the game tree used for MCTS
	Tree* getGameTree() const;

private:
    // const GameState &actualGameState;
    GameState simulatedGameState;
    Player AI_Player, OP_Player; // AI knows about opponent & user player token
    std::unique_ptr<Tree> m_pGameTree;	// the game tree
	////////////////////////////////////////////////////////////////////////

private:
	/// expands all child nodes of passed node (mostly used for root (R)) 
	/// also initializes those created child nodes with a 
	/// randomly assigned high UCTB-value
    //void expandAllChildrenOf(NodeType *Node, const GameState & gp);
	
	/// recursively does the selection step of the mcts (arg node is the root)
	NodeType *selectPromisingNode(NodeType *node);

	/// does the expansion step of the mcts
    NodeType *expandNode(NodeType *leaf_node);


	/**
	 @brief		simulates a game from the expanded node (C) and 
				updates the game tree (just one simulation per call)

	 @note		the simulation is only needed to rate the expanded node (C) and 
				the path to that expanded node

	 @return	a double indicaing if the game was won by the AI

	 * 1	- the game was won by the ai
	 * 0.5	- the game was a draw
	 * 0	- the game was lost by the ai
	 * rewards/ratings taken from 
	 *	http://www.tantrix.com/Tantrix/TRobot/MCTS%20Final%20Report.pdf
	 */
    double simulation(NodeType *expanded_node);
#define VALUE_WIN   10.0;
#define VALUE_DRAW	0.0;
#define VALUE_LOOSE 0.0; 

//#define VALUE_WIN_IMM	3.0;
//#define VALUE_LOOSE_IMM 0.0;
//#define VALUE_FULL_TIE  0.8;


/*	C - curiousity of the algorithm (empirically set to 1.41421L)

	constant C = CURIOUSITY_FACTOR = curiousity of the algorithm
	small C => game tree gets deeper expanded
					(only the best variation gets explored)
	big C => game tree gets broader expanded
					(nodes with lesser visits are prefered)
*/
#define CURIOUSITY_FACTOR 1.41421L
	/// use the result of the playout to update (backpropagate) information in 
	/// the nodes (of the game tree) on the path from 
	/// expanded node (C) to root node (R)
    void backpropagation(NodeType *expanded_node, double ratingToBeUpdated);


	///////////////////////////////////////////////////////////////////////////

private:
	/// selects the most visited child node from the game tree rootNode
    NodeType *selectMostVisitedChild(const NodeType *rootNode);
	NodeType *findBestNodeWithUCT(NodeType *node);
	double uctValue(NodeType* node);

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

	// macht einen zuflligen spielzug und ndert gameDataH
	// es wird die position des Spielsteins zurckgegeben
	/**
	* @brief	do a random move
	* @return	the move (aka the chosen column)
	* @note		changes the passed GameState
	*/
    int doRandomMove(GameState &gp);
};