#include "GameAI.h"

///////////////////////////////////////////////////////////////////////////


GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI) {
	if (AI_Player == PLAYER_1)
		OP_Player = PLAYER_2;
	else
		OP_Player = PLAYER_1;
}

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
	int chosenTurn = chosen_node->chosenTurnThatLeadedToThisNode;
	return chosenTurn;
}

Tree* GameAI::getGameTree() const
{
	return m_pGameTree.get();
}

///////////////////////////////////////////////////////////////////////////

int GameAI::pickBestMove(const GameState &gs) {
    int col_move;

    // try to look for a winning move
    for (col_move = 0; col_move < gs.getMAX_X(); ++col_move) {
        GameState newState = gs;  // make a mutable copy of the const GameState
        newState.insertTokenIntoColumn(col_move); // make the move on the copy

        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == AI_Player) {
            return col_move; // the AI should do this move to win!
        }
    }

	// try to look for a move that hinders the opponent from winning
    for (col_move = 0; col_move < gs.getMAX_X(); ++col_move) {
        GameState newState = gs;  // make mutable copy of the const GameState
		newState.setTurnPlayer(OP_Player);	// simulate a turn of the opponent
        newState.insertTokenIntoColumn(col_move); // make the move on the copy

        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == OP_Player) {
			return col_move; 
			// opponent player will probably make this move to win in his turn, 
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
    srand((unsigned int) t);

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

void GameAI::expandAllChildrenOf(NodeType *nodeToExpand, const GameState & gs) {
    // go through all possible moves:

	GameState tmpState;	// @todo bad performance to copy over all again
    for (int col = 0; col < tmpState.getMAX_X(); ++col) {
		
		tmpState = gs;	// reset of game state

		// try to insert into a column inside the game state
        int columnChosen = col;
        int col_err = tmpState.insertTokenIntoColumn(columnChosen);

		// update the game tree to hold expanded nodes
        if (col_err != NO_VALID_MOVE) {
            NodeType *newNode = m_pGameTree->createNewNode();
            newNode->UCTB = rand() % 1000 + 10000;
            newNode->chosenTurnThatLeadedToThisNode = columnChosen;
			newNode->sequenceThatLeadedToThisNode = "root.move" 
				+ std::to_string(newNode->chosenTurnThatLeadedToThisNode);
			++nodeToExpand->visits;
			++newNode->visits;

            Tree::addNodeTo(newNode, nodeToExpand);
        }
    }
}

/////////////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeType * GameAI::selection(NodeType *rootNode)
{
	NodeType *selectedNode = rootNode;

	// select nodes with the highest UCTB-value and 
	// advance like that until a leaf node L is reached
	while (!(selectedNode->childNodes.empty()))
	{
		// go through all childNodes and choose the one with the highest UCTB
		NodeType *next = nullptr;
		double max_uctb = 0.0f;
		for (size_t i = 0; i < (selectedNode->childNodes).size(); ++i) {
			if (max_uctb <= (selectedNode->childNodes[i])->UCTB) {
				max_uctb = (selectedNode->childNodes[i])->UCTB;
				next = (selectedNode->childNodes[i]);
			}
		}
		selectedNode = next; //advance selectedNode to one of its UCTB-children

		// replay what has been already analysed inside the game tree
		simulatedGameState.insertTokenIntoColumn(
			selectedNode->chosenTurnThatLeadedToThisNode
		);
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
		hasWonSim == BOTH) {
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
    newNodeC->chosenTurnThatLeadedToThisNode = columnChosen;
	newNodeC->sequenceThatLeadedToThisNode = 
		leaf_node->sequenceThatLeadedToThisNode + ".move" 
		+ std::to_string(columnChosen);
	
    // e4.) add the newly created Node to the leaf node L
    Tree::addNodeTo(newNodeC, leaf_node);

    return newNodeC; // = expanded_node C
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
    return VALUE_TIE;
}

///////////////////////////////////////
/// B A C K P R O P A G A T I O N ///
/////////////////////////////////////
void GameAI::backpropagation(NodeType *expanded_node,double ratingToBeUpdated){


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
    } while (cur_node->parent != NULL);

	// ensure that root visit and rating are updated as well: 
	// cur_node == rootNode here!
	++(cur_node->visits);
	cur_node->value += ratingToBeUpdated;
}

// ::: exploitation :::
double GameAI::calculateWinRatio(const NodeType& cur_node) const {
	return cur_node.value / cur_node.visits;
}

// ::: exploration :::
double GameAI::calculateUCT(const NodeType& cur_node) const {
	/*
		constant C = CURIOUSITY_FACTOR = curiousity of the algorithm
		small C => game tree gets deeper expanded
						(only the best variation gets explored)
		big C => game tree gets broader expanded
						(nodes with lesser visits are prefered)
	*/
	// protects against division by 0
	if (cur_node.visits == 0) { return 0.0; }

	return CURIOUSITY_FACTOR * sqrt(
		static_cast<double>(log(cur_node.parent->visits))
		/ (cur_node.visits)
	);
}
double GameAI::calculateUCTB(const NodeType& cur_node) const {
	return calculateWinRatio(cur_node) + calculateUCT(cur_node);
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