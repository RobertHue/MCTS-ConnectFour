///////////////////////////////////////////////////////////////////////////
#include "GameAI.h"
#include <limits> // for numeric::limits to get the smallest double
#include <stdexcept>
#include <iterator> // for std::advance

std::ostream & operator<<(std::ostream & os, const NodeDataType & node)
{
	os << node.sequenceThatLeadedToThisNode << " | " << node.rating << "/" << node.visits << '\t';
	return os;	// enables concatenation of ostreams with <<
}

inline void printChildNodeInfo(const NodeType * node) {
	int firstTab = 15;
	int secondTab = 8;
	std::cout << "-----------------------\n";
	std::cout << "Child Node Info:\n";
	for (auto& n : node->childNodes) {

		std::cout << "[ Move:" << n->data.chosenMoveThatLeadedToThisNode
			<< std::setw(firstTab) << "Wins:"		<< std::setw(secondTab) << n->data.rating
			<< std::setw(firstTab) << "Visits:"		<< std::setw(secondTab) << n->data.visits
			<< std::setw(firstTab) << "UCTB:"		<< std::setw(secondTab) << n->data.UCTB
			<< std::setw(firstTab) << "winratio:"	<< std::setw(secondTab) << n->data.winratio
			<< std::setw(firstTab) << "uct:"		<< std::setw(secondTab) << n->data.uct
			<< "]\n";
	}
	std::cout << "-----------------------\n";
}

GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI) {
	if (AI_Player == Player::PLAYER_1)
		OP_Player = Player::PLAYER_2;
	else
		OP_Player = Player::PLAYER_1;
}

int GameAI::findNextMove(const GameState &gameState) {
	m_pGameTree.reset(new TreeType());

	std::cout << "Start of MCTS!" << std::endl;
	NodeType *selected_node;
	NodeType *expanded_node;
	Value rating;

	// @todo - make a mutable copy of the const GameState (deep copy)
	simulatedGameState = gameState;

	// @todo - can be used but for testing purposes not currently being used:
	expandAllChildrenOf(m_pGameTree->getRoot());
	for (size_t i = 0; i < MAX_NUM_OF_ITERATIONS; ++i) {

		// reset the simulated GameState
		// make a mutable copy of the const GameState (deep copy)
		simulatedGameState = gameState; 

		//@note:   root-node is always the opponent's turn (OP_Player), 
		//which is also the current turn, so now the AI can chose it's turn

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		
 		selected_node = selectPromisingNode(m_pGameTree->getRoot());

		expanded_node = expandNode(selected_node);	// expansion can also be performed after the simulation..

		rating = simulation(expanded_node);

		backpropagation(expanded_node, rating);

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		//@todo getAvailablePositions, getEmptyPosition, 
		// debug:
		// TreeType::levelOrder(m_pGameTree->getRoot());
		// GameState::drawGameStateOnConsole(simulatedGameState.getGameData(), 
		// simulatedGameState.getMAX_X(), simulatedGameState.getMAX_Y());
		// system("PAUSE");
	} 
	// debug:
	//TreeType::printLevelOrder(m_pGameTree->getRoot());
	m_pGameTree->printLevelOrder();
	printChildNodeInfo(m_pGameTree->getRoot());
 
	std::cout << "End of MCTS!" << std::endl;
	NodeType *chosenNode = selectMostVisitedChild(m_pGameTree->getRoot());
	int chosenMove = chosenNode->data.chosenMoveThatLeadedToThisNode;
	return chosenMove;
}


void GameAI::expandAllChildrenOf(NodeType *nodeToExpand) {
	std::vector<int> possibleMoves = setupPossibleMovesOf(nodeToExpand);
	
	Player currentPlayer = (nodeToExpand->data.player == Player::PLAYER_1)
		? Player::PLAYER_2 : Player::PLAYER_1;

    for (int pmove : possibleMoves) {
		// update the game TreeType to hold expanded nodes
		NodeType *newNode = m_pGameTree->createNewNode(nodeToExpand);
		newNode->data.UCTB = rand() % 1000 + 10000;
		newNode->data.chosenMoveThatLeadedToThisNode = pmove;
		newNode->data.sequenceThatLeadedToThisNode =
			nodeToExpand->data.sequenceThatLeadedToThisNode + "."
			+ std::to_string(newNode->data.chosenMoveThatLeadedToThisNode);
		newNode->data.level = nodeToExpand->data.level + 1;
		newNode->data.player = currentPlayer;
    }
}

NodeType* GameAI::findBestNodeWithUCT(NodeType* node) {
	// go through all childNodes and choose the one with the highest UCTB
	NodeType *nodeWithHighestUCT = nullptr; // is possible when childNodes are empty
	
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
	if(node->data.visits == 0) {
		return std::numeric_limits<double>::max();
	}
	
	// ::: exploitation :::
	node->data.winratio = node->data.rating / node->data.visits;

	// ::: exploration :::
	node->data.uct = CURIOUSITY_FACTOR * sqrt(
		static_cast<double>(log(node->parent->data.visits)) / static_cast<double>(node->data.visits)
	);

	// calculate UCTB value
	node->data.UCTB = node->data.winratio + node->data.uct;

	return node->data.UCTB;
}

/////////////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeType * GameAI::selectPromisingNode(NodeType *rootNode)
{
	NodeType* selectedNode = rootNode;
	
	while(!(selectedNode->childNodes.empty()))
	{
		selectedNode = findBestNodeWithUCT(selectedNode);
		
		if(selectedNode == nullptr) {
			throw std::invalid_argument("nodeTemp is nullptr");
		}
		
		// replay what has been already analysed inside the game TreeType
		simulatedGameState.insertTokenIntoColumn(
			selectedNode->data.chosenMoveThatLeadedToThisNode
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
	// if a level node having 10 visits at depth level of 1,
	// then expand all of its child nodes
	//if ((leaf_node->visits + 1) >= EXPAND_FULLY_ON_VISITS && leaf_node->level >= 0) {
	//	expandAllChildrenOf(leaf_node);
	//}

	// e1.) Does the Leaf Node L node end the Game? (won/loss/tie)?
    Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == Player::PLAYER_1 ||
		hasWonSim == Player::PLAYER_2 ||
		hasWonSim == Player::DRAW) {
		return leaf_node;  // if so, don't expand!
	}

	//////////////////////////////////////

	std::vector<int> possibleMoves = setupPossibleMovesOf(leaf_node);
	
	// sim1.) choose a good move from the movePossibilites (columnChosen)
	// int columnChosen = pickRandomMoveFrom(possibleMoves, simulatedGameState);
	int columnChosen = pickBestMoveFrom(possibleMoves, simulatedGameState);
	if (columnChosen == -1) {
		return leaf_node;  //if game panel is full => FULL_TIE & stop sim
	}

	// sim2.) do the selected move (columnChosen)
	Player currentPlayer = simulatedGameState.getTurnPlayer();
	simulatedGameState.insertTokenIntoColumn(columnChosen);

	// e2.) expansion: create a newNode
	NodeType *newNodeC = m_pGameTree->createNewNode(leaf_node);
	newNodeC->data.chosenMoveThatLeadedToThisNode = columnChosen;
	newNodeC->data.sequenceThatLeadedToThisNode =
		leaf_node->data.sequenceThatLeadedToThisNode + "."
		+ std::to_string(columnChosen);
	newNodeC->data.level = leaf_node->data.level + 1;
	newNodeC->data.player = currentPlayer;

	return newNodeC; // return expanded_node C
}

 /////////////////////////////////
 /// S I M U L A T I O N ///
 ///////////////////////////
double GameAI::simulation(NodeType *expanded_node) {
    // has someone won?
    Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == AI_Player) {
        return Value::WIN; // AI has won immediately => stop simulating
	} else if (hasWonSim == OP_Player) {
		return Value::LOOSE; // OP has won immediately => stop simulating
	} else if (hasWonSim == Player::DRAW) {
        return Value::DRAW;
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
            return Value::DRAW; // if game panel is full => FULL_TIE & stop sim
        }
        // sim2.) chose a move
        simulatedGameState.insertTokenIntoColumn(columnChosen);
			   
        // has someone won?
        Player hasWonSim = simulatedGameState.hasSomeoneWon();
        if (hasWonSim == AI_Player) {
            return Value::WIN; // AI has won => stop simulating
        } else if (hasWonSim == OP_Player) {
            return Value::LOOSE; // OP has won => stop simulating
        } else if (hasWonSim == Player::DRAW) {
			return Value::DRAW;
		}
	
    }
    return Value::DRAW;
}

///////////////////////////////////////
/// B A C K P R O P A G A T I O N ///
/////////////////////////////////////
void GameAI::backpropagation(NodeType *expanded_node, const Value ratingToBeUpdated) {
	// if rating is a draw then add it to every node upwards
	bool switchSameRating = (ratingToBeUpdated == Value::DRAW) ? true : false;

	Value ratingToBeUpdatedAI = Value::DRAW;
	Value ratingToBeUpdatedOther = Value::DRAW;

	if (!switchSameRating) {
		ratingToBeUpdatedAI = ratingToBeUpdated;
		ratingToBeUpdatedOther = (ratingToBeUpdated == Value::WIN) ?
			Value::LOOSE : Value::WIN;
	}

	///////////////////////////////////////////////////////////////////////////
	//update visit and rating values upwards from expanded node C to L to root
	NodeType *cur_node = expanded_node;
	while (cur_node != nullptr) { 
		if (switchSameRating) {
			cur_node->data.rating += ratingToBeUpdated;
		}
		else {	// ratingToBeUpdated != Value::DRAW
			if (cur_node->data.player == this->AI_Player) { cur_node->data.rating += ratingToBeUpdatedAI; }
			else if (cur_node->data.player == this->OP_Player) { cur_node->data.rating += ratingToBeUpdatedOther; }
		}

		//-------------------------------------------------------------------
		++(cur_node->data.visits);			// increase the visits of every node
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
        if (max_visit_count <= currentChildNode->data.visits) {
            max_visit_count = currentChildNode->data.visits;
			selected_node = currentChildNode;
        }
    }
    return selected_node;
}

///////////////////////////////////////////////////////////////////////////

int GameAI::pickBestMoveFrom(std::vector<int>& possibleMoves, const GameState &gs) {
	Player currentPlayer = gs.getTurnPlayer();
	Player OtherPlayer = gs.getOtherPlayer();

	// try to look for a winning move for current player
	for (std::size_t i = 0; i < possibleMoves.size(); ++i) {
		GameState tmpState = gs;  // make a mutable copy of the const GameState
		tmpState.insertTokenIntoColumn(possibleMoves[i]); // make the move on the copy

		Player playerThatJustWon = tmpState.hasSomeoneWon();
		if (playerThatJustWon == currentPlayer) {
			return possibleMoves[i]; // the currentPlayer should do this move to win!
		}
	}

	// try to look for a move that hinders the OtherPlayer from winning
	for (std::size_t i = 0; i < possibleMoves.size(); ++i) {
		GameState tmpState = gs;  // make mutable copy of the const GameState
		tmpState.setTurnPlayer(OtherPlayer);	// simulate a turn of the opponent
		tmpState.insertTokenIntoColumn(possibleMoves[i]); // make the move on the copy

		Player playerThatJustWon = tmpState.hasSomeoneWon();
		if (playerThatJustWon == OtherPlayer) {
			return possibleMoves[i];
			// OtherPlayer will probably make this move to win in his turn, 
			// so hinder him on doing that!
		}
	}

	// here: No winning move found or hindering needed, so pick a random move
	return pickRandomMoveFrom(possibleMoves, gs);
}

int GameAI::pickRandomMoveFrom(std::vector<int>& possibleMoves, const GameState &gs) {
	/*
	std::vector<std::vector<int>> gameData = gs.getGameData();
	// collect every column, where a token can be put into:
	std::vector<int> possibleColumns;
	for (int i = 0; i < gs.getMAX_X(); ++i) {
		if (gameData[i][0] == FREE_FIELD) {
			possibleColumns.push_back(i);
		}
	}
	if (possibleColumns.empty()) { return -1; } // all columns are already full
	*/
	/* initialize random seed: */
	time_t t;
	time(&t);
	srand((unsigned int)t);

	/* generate secret number between 1 and MAX_X: */
	int iRand = rand() % possibleMoves.size() + 0;

	int randomColumn = possibleMoves[iRand];
	return randomColumn;
}

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
		if (gameData[i][0] == static_cast<int>(Player::NONE)) {
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
		if (n->data.chosenMoveThatLeadedToThisNode == columnChosen) {
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
	if (pickedColumn != -1) { gs.insertTokenIntoColumn(pickedColumn); }
	return pickedColumn;
}

std::vector<int> GameAI::setupPossibleMovesOf(NodeType * node) {
	// make a deep copy of the simulatedGameState
	GameState tmpState = simulatedGameState;

	// setup the possible moves:
	std::vector<int> possibleMovesToExpand = tmpState.getPossibleMoves();
	for (auto& n : node->childNodes) {
		std::vector<int>::const_iterator citer = possibleMovesToExpand.cbegin();
		for (auto & p : possibleMovesToExpand) {
			if (p == n->data.chosenMoveThatLeadedToThisNode) {
				possibleMovesToExpand.erase(citer);
				break;	// break out of possibleMovesToExpand loop
			}
			++citer;
		}
	}
	return possibleMovesToExpand;
}

///////////////////////////////////////////////////////////////////////////

TreeType* GameAI::getGameTree() const
{
	return m_pGameTree.get();
}

///////////////////////////////////////////////////////////////////////////