///////////////////////////////////////////////////////////////////////////
#include "GameAI.h"
#include <limits> // for numeric::limits to get the smallest double
#include <stdexcept>


GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI) {
	if (AI_Player == PLAYER_1)
		OP_Player = PLAYER_2;
	else
		OP_Player = PLAYER_1;
}

int GameAI::findNextMove(const GameState &gameState) {
	m_pGameTree.reset(new Tree());

	std::cout << "Start of MCTS!" << std::endl;
	NodeType *selected_node;
	NodeType *expanded_node;
	double rating;

	// @todo - make a mutable copy of the const GameState (deep copy)
	// simulatedGameState = gameState;

	// @todo - can be used but for testing purposes not currently being used:
	// expandAllChildrenOf(m_pGameTree->getRoot(), gameState);
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

		selected_node = selectPromisingNode(m_pGameTree->getRoot());

		// check whether selected_node_L is win/loose/draw:
		// has someone won?
		Player hasWonSim = simulatedGameState.hasSomeoneWon();
		double ratingINbetween = 0;
		bool toggle = false;
		if (hasWonSim == AI_Player) {
			VALUE_WIN; // AI has won immediately => stop simulating
			ratingINbetween = toggle = true;
		}
		else if (hasWonSim == OP_Player) {
			 VALUE_LOOSE; // OP has won immediately => stop simulating
			 ratingINbetween = toggle = true;
		}
		else if(hasWonSim == DRAW) {
			VALUE_LOOSE; // OP has won immediately => stop simulating
			ratingINbetween = toggle = true;
		}

		if (!toggle) {
			//expanded_node = expandNode(selected_node);
			expandAllChildrenOf(selected_node, simulatedGameState);
			expanded_node = pickBestChild(selected_node, simulatedGameState);
			if (expanded_node == nullptr) { expanded_node = selected_node; }
			else {
				simulatedGameState.insertTokenIntoColumn(expanded_node->chosenMoveThatLeadedToThisNode);
			}

			rating = simulation(expanded_node);
		}
		else {
			expanded_node = selected_node;
			rating = ratingINbetween;
		}
		
		backpropagation(expanded_node, rating);

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		//@todo getAvailablePositions, getEmptyPosition, 
		// debug:
		// Tree::levelOrder(m_pGameTree->getRoot());
		// GameState::drawGameStateOnConsole(simulatedGameState.getGameData(), 
		// simulatedGameState.getMAX_X(), simulatedGameState.getMAX_Y());
		// system("PAUSE");
	} 
	// debug:
	//Tree::printLevelOrder(m_pGameTree->getRoot());
	Tree::printChildNodeInfo(m_pGameTree->getRoot());

 
	std::cout << "End of MCTS!" << std::endl;
	NodeType *chosenNode = selectMostVisitedChild(m_pGameTree->getRoot());
	int chosenMove = chosenNode->chosenMoveThatLeadedToThisNode;
	return chosenMove;
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
					newNode->sequenceThatLeadedToThisNode = 
						nodeToExpand->sequenceThatLeadedToThisNode + ".move"
						+ std::to_string(newNode->chosenMoveThatLeadedToThisNode);

					Tree::addNodeTo(newNode, nodeToExpand);
			}
        }
    }
}

NodeType* GameAI::findBestNodeWithUCT(NodeType* node) {
	// go through all childNodes and choose the one with the highest UCTB
	NodeType *nodeWithHighestUCT = nullptr;
	
	double max_uctb = -std::numeric_limits<double>::max();
	for (auto& n : node->childNodes) {
		double curUCTB = uctValue(n);
		if (max_uctb <= curUCTB) {
			max_uctb = curUCTB;
			nodeWithHighestUCT = n;
		}
	}
	return nodeWithHighestUCT;
}

double GameAI::uctValue(NodeType* node) {
	// protect against division by zero
	if(node->visits == 0) {
		return std::numeric_limits<double>::max();
	}
	
	// ::: exploitation :::
	node->winratio = node->rating / node->visits;

	// ::: exploration :::
	node->uct = CURIOUSITY_FACTOR * sqrt(	
		static_cast<double>(log(node->parent->visits)) / static_cast<double>(node->visits)
	);

	// calculate UCTB value
	node->UCTB = node->winratio + node->uct;

	return node->UCTB; 
}

/////////////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeType * GameAI::selectPromisingNode(NodeType *rootNode)
{
	NodeType* selectedNode = rootNode;
	
	while(!(selectedNode->childNodes.empty()))
	{
		NodeType *nodeTemp = findBestNodeWithUCT(selectedNode);
		
		if(nodeTemp == nullptr) {
			throw std::invalid_argument("nodeTemp is nullptr");
		}
		
		selectedNode = nodeTemp;
		
		// replay what has been already analysed inside the game tree
		simulatedGameState.insertTokenIntoColumn(
			selectedNode->chosenMoveThatLeadedToThisNode
		);
	}
	
	return selectedNode;
}
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////
/// E X P A N S I O N ///
/////////////////////////	// L -> C
NodeType * GameAI::expandNode(NodeType *leaf_node)
{
	// e1.) Does the Leaf Node L node end the Game? (won/loss/tie)?
    Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == PLAYER_1 || 
		hasWonSim == PLAYER_2 || 
		hasWonSim == DRAW) {
		return leaf_node;  // if so, don't expand!
	}
	
	// sim1.) choose a good move (columnChosen)
	int columnChosen = pickBestMove(simulatedGameState);
	if (columnChosen == -1) {
		return leaf_node;  //if game panel is full => FULL_TIE & stop sim
	}
	
	// sim2.) do the selected move (columnChosen)
	simulatedGameState.insertTokenIntoColumn(columnChosen);

	// e2.) expansion: create a newNode
	NodeType *newNodeC = m_pGameTree->createNewNode();
	newNodeC->chosenMoveThatLeadedToThisNode = columnChosen;
	newNodeC->sequenceThatLeadedToThisNode =
		leaf_node->sequenceThatLeadedToThisNode + ".move"
		+ std::to_string(columnChosen);
		
	// e4.) add the newly created Node to the leaf node L
	Tree::addNodeTo(newNodeC, leaf_node);
	
	return newNodeC; // return expanded_node C
}

 /////////////////////////////////
 /// S I M U L A T I O N ///
 ///////////////////////////
double GameAI::simulation(NodeType *expanded_node) {
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
		// int columnChosen = pickRandomMove(simulatedGameState);
		int columnChosen = pickBestMove(simulatedGameState);
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
    return VALUE_DRAW;
}

///////////////////////////////////////
/// B A C K P R O P A G A T I O N ///
/////////////////////////////////////
void GameAI::backpropagation(NodeType *expanded_node, double ratingToBeUpdated) {
	bool toggleTurn = true;
	///////////////////////////////////////////////////////////////////////////
	//update visit and rating values upwards from expanded node C to L to root
	NodeType *cur_node = expanded_node;
	while (cur_node != nullptr) {
		if (0.5 == (ratingToBeUpdated)) {	// if rating is a draw then add it to every node upwards
			cur_node->rating += ratingToBeUpdated;
			toggleTurn = false;
		} 
		
		if (toggleTurn) {	// add rating to every 2nd node
			cur_node->rating += ratingToBeUpdated;
		}

		++(cur_node->visits);		
		//-------------------------------------------------------------------
		toggleTurn = !toggleTurn;
		cur_node = cur_node->parent;	// advance upwards
	}
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


NodeType * GameAI::selectMostVisitedChild(const NodeType *rootNode) {
    NodeType *selected_node = nullptr;
    int max_visit_count = 0;

    // inspect all children visits and store node with max visits
    for (auto currentChildNode : rootNode->childNodes) //for all children
    {
        if (max_visit_count <= currentChildNode->visits) {
            max_visit_count = currentChildNode->visits;
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

NodeType * GameAI::pickBestChild(NodeType *node, const GameState &gs) {
	GameState tmpState = gs;  // make mutable copy of the const GameState

	int columnChosen = pickBestMove(tmpState);
	if (columnChosen == -1) {
		return pickRandomChild(node); // if game panel is full => pick random child
	}

	NodeType *bestChild = nullptr;
	for (auto& n : node->childNodes) {
		if (n->chosenMoveThatLeadedToThisNode == columnChosen) {
			bestChild = n;
		}
	}

	if(bestChild == nullptr) { return pickRandomChild(node); }
	return bestChild;
}
NodeType * GameAI::pickRandomChild(NodeType *node) {
	if (node->childNodes.size() > 0) {
		/* initialize random seed: */
		time_t t;
		time(&t);
		srand((unsigned int)t);

		/* generate secret number between 1 and MAX_X: */
		int iRand = rand() % node->childNodes.size() + 0;

		NodeType *randomNode = node->childNodes[iRand];
		return randomNode;
	}
	return nullptr;
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