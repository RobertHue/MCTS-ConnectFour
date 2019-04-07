#pragma once

#include <queue>
#include <stack>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "GamePanel.h"
#include "Tree.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
using boost::property_tree::ptree;




// @TODO kleine beschreibung der klasse? was kann man damit machen? wie funktioniert sie grob`?
/// for more information on the mcts, see:
///		http://de.slideshare.net/ftgaic/mcts-ai
///		https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
class GameAI {
public:

	/// construct a GameAI for the given Player and let it take control over him
	/// call calculateNextTurn to make the AI do the next move for the Player
    GameAI(Player tokenKI);

	/// calculates the next move for the AI based on the MCTS-algorithm
	/// @return		the MCTS-based move for the AI
    int calculateNextTurn(const GamePanel &gPanel);

private:
    size_t MAX_NUM_OF_ITERATIONS = 2000; // the number of iterations (each iteration resembles a simualation of one complete game)
    // const GamePanel &actualGamePanel;
    GamePanel simulatedGamePanel;
    Player AI_Player, OP_Player; // The AI knows about opponent and user player token
    Tree gameTree;	// the game tree @TODO can be replaced with the boost property tree
	//ptree m_pt; // @TODO future game tree ?

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// highly coupled methods depending on the kind of game (here connect four) ////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 * @brief	tries to pick the best move it can do for the next turn (pretty greedy)
	 *			if not possible, it will pick a random placement
	 *
	 * @Implementation 
	 *		based on TRIAL AND ERROR:
	 * test each column whether a move will lead to a win for the turnPlayer (doesn't matter whether KI or human player)
	 * it needs to simulate turns so that counts for the simulated turn of opponent too (the KI predicts what his opponent will be doing)
	 * If no winning turn is possible or 
	 * the avoidance of opponent winning is not possible with this move,
	 * then select a random move
	 *
	 * @Note: this function does not change the passed GamePanel-object (const)
	 */
    int pickBestMove(const GamePanel &gp);

	/**
	 * @brief:	picks a random move in column
	 * @return: the column col (0-7)
	 *		returns -1 when every column is already full
	 */
    int pickRandomMove(const GamePanel &gp);

	// macht einen zuflligen spielzug und ndert gameDataH
	// es wird die position des Spielsteins zurckgegeben
	/**
	* @brief	do a random move
	* @return	the move (aka the chosen column)
	* @note		changes the passed GamePanel
	*/
    int doRandomMove(GamePanel &gp);

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	/// expands all child nodes of passed node (mostly used for root (R)) 
	/// also initializes those created child nodes with a randomly assigned high UCTB-value
    void expandAllChildrenOf(NodeType *Node);
	
	/// recursively does the selection step of the mcts
    NodeType *recursive_selection(NodeType *Node);

	/// does the expansion step of the mcts
    NodeType *expansion(NodeType *leaf_node);

#define VALUE_WIN_IMM  3000.0;
#define VALUE_LOOSE_IMM  0.0;
#define VALUE_WIN   1.0;
#define VALUE_FULL_TIE  0.0;
#define VALUE_TIE   0.5;
#define VALUE_DRAW	VALUE_TIE;
#define VALUE_LOOSE   0.0;

// curiousity of the algorithm (mostly squareroot of 2 is chosen)
#define CURIOUSITY_FACTOR 1.41421L
	/**
	 @brief		simulates a game from the expanded node (C) and updates the game tree

	 @note		the simulation is only needed to rate the expanded node (C) and the path to that expanded node

	 @return	a double indicaing if the game was won by the AI

	 * 1	- the game was won by the ai
	 * 0.5	- the game was a draw
	 * 0	- the game was lost by the ai
	 * rewards/ratings taken from http://www.tantrix.com/Tantrix/TRobot/MCTS%20Final%20Report.pdf
	 */
    double simulation(NodeType *expanded_node);

	/// use the result of the playout to update (backpropagate) information in the nodes (of the game tree) on the path from expanded node (C) to root node (R)
    void backpropagation(NodeType *expanded_node, double ratingToBeUpdated);

	///////////////////////////////////////////////////////////////////////////

	/// selects a save child node of passed node from the game tree
    NodeType *selectSaveChild(NodeType *Node);

	/// selects a robust child node of passed node from the game tree
    NodeType *selectRobustChild(NodeType *Node);

};
