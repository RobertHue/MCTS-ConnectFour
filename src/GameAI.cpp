#include "GameAI.h"

///////////////////////////////////////////////////////////////////////////
#include <limits> // for numeric::limits to get the smallest double

GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI) {
	if (AI_Player == PLAYER_1)
		OP_Player = PLAYER_2;
	else
		OP_Player = PLAYER_1;
}
// @todo findNextMove
int GameAI::calculateNextTurn(const GameState &gameState) {
	m_pGameTree.reset(new Tree());

	std::cout << "Start of MCTS!" << std::endl;
	NodeType *selected_node;
	NodeType *expanded_node;
	double rating;

	// make a mutable copy of the const GameState (deep copy)
	simulatedGameState = gameState;

	expandAllChildrenOf(m_pGameTree->getRoot(), gameState);
	for (size_t i = 0; i < MAX_NUM_OF_ITERATIONS; ++i) {
		// debug:
		//Tree::printLevelOrder(m_pGameTree->getRoot());
		//Tree::printChildNodeInfo(m_pGameTree->getRoot());

		// reset the simulated GameState
		// make a mutable copy of the const GameState (deep copy)
		simulatedGameState = gameState; 

		//@note:   root-node is always the opponent's turn (OP_Player), 
		//which is also the current turn, so now the AI can chose it's turn

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////

		selected_node = selection(m_pGameTree->getRoot());

		expanded_node = expansion(selected_node);

		rating = simulation(expanded_node);
		
		backpropagation(expanded_node, rating);

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////

		// debug:
		// Tree::levelOrder(m_pGameTree->getRoot());
		// GameState::drawGameStateOnConsole(simulatedGameState.getGameData(), 
		// simulatedGameState.getMAX_X(), simulatedGameState.getMAX_Y());
		// system("PAUSE");
	}
	// debug:
	//Tree::printLevelOrder(m_pGameTree->getRoot());
	//Tree::printChildNodeInfo(m_pGameTree->getRoot());


	std::cout << "End of MCTS!" << std::endl;
	NodeType *chosen_node = selectMostVisitedChild(m_pGameTree->getRoot());
	int chosenTurn = chosen_node->chosenMoveThatLeadedToThisNode;
	return chosenTurn;
}



void GameAI::expandAllChildrenOf(NodeType *nodeToExpand, const GameState & gs) {
    // go through all possible moves:

	std::vector<int> movesToExclude;
	for (auto n : nodeToExpand->childNodes) {	// has children already
		movesToExclude.push_back(n->chosenMoveThatLeadedToThisNode);
	}


	GameState tmpState;	// @todo bad performance to copy over all again
    for (int col = 0; col < tmpState.getMAX_X(); ++col) {
		bool isSaveToExpand = true;

		// go through movesToExlude to avoid double expanding:
		for (auto & m : movesToExclude) {
			if (m == col) {
				isSaveToExpand = false;
				break;	// break free out of loop and continue
			}
		}

		if (isSaveToExpand) {
			tmpState = gs;	// reset of game state

			// try to insert into a column inside the game state
			int columnChosen = col;
			int col_err = tmpState.insertTokenIntoColumn(columnChosen);

			// update the game tree to hold expanded nodes
			// @todo has to check whether node is already expanded...
			if (col_err != NO_VALID_MOVE) {
					NodeType *newNode = m_pGameTree->createNewNode();
					newNode->UCTB = rand() % 1000 + 10000;
					newNode->chosenMoveThatLeadedToThisNode = columnChosen;
					newNode->sequenceThatLeadedToThisNode = "root.move"
						+ std::to_string(newNode->chosenMoveThatLeadedToThisNode);

					Tree::addNodeTo(newNode, nodeToExpand);
			}
        }
    }
}

/////////////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeType * GameAI::selection(NodeType *rootNode)
{
	++(rootNode->visits);
	NodeType *selectedNode = rootNode;

	int level = 0;
	// select nodes with the highest UCTB-value and 
	// advance like that until a leaf node L is reached
	while (!(selectedNode->childNodes.empty()))
	{
		++level;
		// go through all childNodes and choose the one with the highest UCTB
		NodeType *next = nullptr;
		double max_uctb = -std::numeric_limits<double>::max();
		for (size_t i = 0; i < (selectedNode->childNodes).size(); ++i) {
			if (max_uctb <= (selectedNode->childNodes[i])->UCTB) {
				max_uctb = (selectedNode->childNodes[i])->UCTB;
				next = (selectedNode->childNodes[i]);
			}
		}
		selectedNode = next; //advance selectedNode to one of its UCTB-children

		/*
		NodeType* tmp = selectedNode;
		max_uctb = -std::numeric_limits<double>::max();
		if (selectedNode == nullptr) {
			for (size_t i = 0; i < (tmp->childNodes).size(); ++i) {
				if (max_uctb <= (tmp->childNodes[i])->UCTB) {
					max_uctb = (tmp->childNodes[i])->UCTB;
				}
			}
		}*/

		// replay what has been already analysed inside the game tree
		simulatedGameState.insertTokenIntoColumn(
			selectedNode->chosenMoveThatLeadedToThisNode
		);

		++(selectedNode->visits); 
		if (selectedNode->visits >= T && level == 1) {
			expandAllChildrenOf(selectedNode, simulatedGameState);
		}
	}
	// leaf node L reached (termination condition reached)
	return selectedNode;
}
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////
/// E X P A N S I O N ///
/////////////////////////	// L -> C
NodeType * GameAI::expansion(NodeType *leaf_node)
{
    // e1.) Does the Leaf Node L node end the Game? (won/loss/tie)?
    Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == PLAYER_1 || 
		hasWonSim == PLAYER_2 || 
		hasWonSim == DRAW) {
		return leaf_node;  // if so, don't expand!
	}

	////////////////////////////////////////////////////
	// Create one of L's child nodes
	// sim1.) choose a good move (columnChosen)
	int columnChosen = pickBestMove(simulatedGameState);
	if (columnChosen == -1) {
		return leaf_node;  //if game panel is full => FULL_TIE & stop sim
	}
	// sim2.) do the selected move (columnChosen)
	simulatedGameState.insertTokenIntoColumn(columnChosen);

	NodeType * nodeToBeChosen = nullptr;
	bool isSafeToExpand = true;
	std::vector<int> movesToExclude;
	for (auto n : leaf_node->childNodes) {	// has children already
		if (n->chosenMoveThatLeadedToThisNode == columnChosen) {
			isSafeToExpand = false;
			nodeToBeChosen = n;
			break;
		}
	}

	if (isSafeToExpand) {
		// e2.) expansion: create a newNode
		NodeType *newNodeC = m_pGameTree->createNewNode();
		newNodeC->chosenMoveThatLeadedToThisNode = columnChosen;
		newNodeC->sequenceThatLeadedToThisNode =
			leaf_node->sequenceThatLeadedToThisNode + ".move"
			+ std::to_string(columnChosen);

		// e4.) add the newly created Node to the leaf node L
		Tree::addNodeTo(newNodeC, leaf_node);
		return newNodeC; // = expanded_node C
	}
	return nodeToBeChosen;
}

 /////////////////////////////////
 /// S I M U L A T I O N ///
 ///////////////////////////
double GameAI::simulation(NodeType *expanded_node) {
	++(expanded_node->visits);

    // has someone won?
    Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == AI_Player) {
        return VALUE_WIN; // AI has won immediately => stop simulating
    } else if (hasWonSim == OP_Player) {
        return VALUE_LOOSE; // OP has won immediately => stop simulating
    }

    // for (int i = 0; i < NUM_OF_SIMULATION_FRAMES; ++i)
    while (1) {
        //GameState::drawGameStateOnConsole(simulatedGameState.getGameData(), 
		//		simulatedGameState.getMAX_X(), simulatedGameState.getMAX_Y());
        //system("PAUSE");

        // choose a move
		int columnChosen = pickRandomMove(simulatedGameState);
		//int columnChosen = pickBestMove(simulatedGameState);
        if (columnChosen == -1) {
            return VALUE_DRAW; // if game panel is full => FULL_TIE & stop sim
        }
        // sim2.) chose a move
        simulatedGameState.insertTokenIntoColumn(columnChosen);
			   
        // has someone won?
        Player hasWonSim = simulatedGameState.hasSomeoneWon();
        if (hasWonSim == AI_Player) {
            return VALUE_WIN; // AI has won => stop simulating
        } else if (hasWonSim == OP_Player) {
            return VALUE_LOOSE; // OP has won => stop simulating
        }
    }
    return VALUE_TIE;
}

///////////////////////////////////////
/// B A C K P R O P A G A T I O N ///
/////////////////////////////////////
void GameAI::backpropagation(NodeType *expanded_node,double ratingToBeUpdated){

	/*
	// b1.) backpropagate from expandednode (C) to Leaf Node (L) and 
	//						other nodes on that way up to root node (R)
    // b3.) update the rating (UCTS) of the nodes up to the root 
	//						(excluding the root)
	NodeType *cur_node = expanded_node;
    do {
        //-------------------------------------------------------------------
		//b2.)increase the amount of visits of each node and add that to rating
		cur_node->value += ratingToBeUpdated;
		++(cur_node->visits);
        //-------------------------------------------------------------------
        cur_node = cur_node->parent;	// advance upwards
    } while (cur_node->parent != nullptr);

	// ensure that root visit and rating are updated as well: 
	// cur_node == rootNode here!
	cur_node->value += ratingToBeUpdated;
	++(cur_node->visits);
	*/
	///////////////////////////////////////////////////////////////
	//update UCTB value
	NodeType *cur_node = expanded_node;
	do {
		cur_node->value += ratingToBeUpdated;

		if (cur_node->visits != 0) {	// protects against division by 0
			/*
				constant C = CURIOUSITY_FACTOR = curiousity of the algorithm
				small C => game tree gets deeper expanded
								(only the best variation gets explored)
				big C => game tree gets broader expanded
								(nodes with lesser visits are prefered)
			*/

			// ::: exploitation :::
			cur_node->winratio = cur_node->value / cur_node->visits;

			// ::: exploration :::
			cur_node->uct = CURIOUSITY_FACTOR
				* sqrt(		// +1 to save against ln(1) = 0 which will lead to unwanted behavior sometimes
					static_cast<double>(log(cur_node->parent->visits+1))
					/ static_cast<double>(cur_node->visits)
				);

			cur_node->UCTB = cur_node->winratio + cur_node->uct;

		}
		else {
			std::cout << "cur node visits == 0 !!!! error!!!" << std::endl;
		}
		//-------------------------------------------------------------------
		cur_node = cur_node->parent;	// advance upwards
	} while (cur_node->parent != nullptr);
	cur_node->value += ratingToBeUpdated; // also for the root
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


NodeType * GameAI::selectMostVisitedChild(NodeType *rootNode) {
    NodeType *selected_node = nullptr;
    int max_visit_count = 0;

    // von node aus alle kinder:
    for (size_t i = 0; i < rootNode->childNodes.size(); ++i) //for all children
    {
        // inspect all children
        NodeType *currentChildNode = rootNode->childNodes[i];

        double save_val = currentChildNode->visits;

        if (max_visit_count <= save_val) { 
            max_visit_count = save_val;
			selected_node = currentChildNode;
        }
    }
    return selected_node;
}

///////////////////////////////////////////////////////////////////////////

int GameAI::pickBestMove(const GameState &gs) {
	int col_move;
	Player currentPlayer = gs.getTurnPlayer();
	Player OtherPlayer = gs.getOtherPlayer();

	// try to look for a winning move for current player
	for (col_move = 0; col_move < gs.getMAX_X(); ++col_move) {
		GameState tmpState = gs;  // make a mutable copy of the const GameState
		tmpState.insertTokenIntoColumn(col_move); // make the move on the copy

		Player playerThatJustWon = tmpState.hasSomeoneWon();
		if (playerThatJustWon == currentPlayer) {
			return col_move; // the currentPlayer should do this move to win!
		}
	}

	// try to look for a move that hinders the OtherPlayer from winning
	for (col_move = 0; col_move < gs.getMAX_X(); ++col_move) {
		GameState tmpState = gs;  // make mutable copy of the const GameState
		tmpState.setTurnPlayer(OtherPlayer);	// simulate a turn of the opponent
		tmpState.insertTokenIntoColumn(col_move); // make the move on the copy

		Player playerThatJustWon = tmpState.hasSomeoneWon();
		if (playerThatJustWon == OtherPlayer) {
			return col_move;
			// OtherPlayer will probably make this move to win in his turn, 
			// so hinder him on doing that!
		}
	}

	// here: No winning move found or hindering needed, so pick a random move
	col_move = pickRandomMove(gs);
	return col_move;
}

int GameAI::pickRandomMove(const GameState &gs) {
	std::vector<std::vector<int>> gameData = gs.getGameData();

	// collect every column, where a token can be put into:
	std::vector<int> possibleColumns;
	for (int i = 0; i < gs.getMAX_X(); ++i) {
		if (gameData[i][0] == FREE_FIELD) {
			possibleColumns.push_back(i);
		}
	}
	if (possibleColumns.empty()) { return -1; } // all columns are already full

	/* initialize random seed: */
	time_t t;
	time(&t);
	srand((unsigned int)t);

	/* generate secret number between 1 and MAX_X: */
	int iRand = rand() % possibleColumns.size() + 0;

	int randomColumn = possibleColumns[iRand];
	return randomColumn;
}

int GameAI::doRandomMove(GameState &gs) {
	int pickedColumn = pickRandomMove(gs);
	if (pickedColumn != -1) gs.insertTokenIntoColumn(pickedColumn);
	return pickedColumn;
}

///////////////////////////////////////////////////////////////////////////

Tree* GameAI::getGameTree() const
{
	return m_pGameTree.get();
}

///////////////////////////////////////////////////////////////////////////