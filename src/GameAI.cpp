#include "GameAI.h"

///////////////////////////////////////////////////////////////////////////

GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI) {
	if (AI_Player == PLAYER_1)
		OP_Player = PLAYER_2;
	else
		OP_Player = PLAYER_1;
}

int GameAI::calculateNextTurn(const GamePanel &gPanel) {
	//gameTree = Tree();

	cout << "Start of MCTS!" << endl;
	NodeType *selected_node;
	NodeType *expanded_node;
	double rating;

	simulatedGamePanel = gPanel; // make a mutable copy of the const GamePanel (deep copy)
	expandAllChildrenOf(gameTree.getRoot());
	for (size_t i = 0; i < MAX_NUM_OF_ITERATIONS; ++i) {
		// reset the simulated GamePanel
		simulatedGamePanel = gPanel; // = echte tiefe Kopie
		// @note:   root-node is always the opponent's turn (OP_Player), which is also the current done turn, so now the AI can chose it's turn

		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		selected_node = recursive_selection(gameTree.getRoot());
		expanded_node = expansion(selected_node);
		rating = simulation(expanded_node);
		backpropagation(expanded_node, rating);
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////

		// debug:
		//Tree::levelOrder(gameTree.getRoot());
		//system("PAUSE");
		// GamePanel::drawGamePanelOnConsole(simulatedGamePanel.getGameData(), simulatedGamePanel.getMAX_X(), simulatedGamePanel.getMAX_Y());
		// system("PAUSE");
		// cout << "X: " << simulatedGamePanel.getPositionOfLastPlacedToken().x << "\nY: " << simulatedGamePanel.getPositionOfLastPlacedToken().y << endl;

	}
	cout << "End of MCTS!" << endl;
	// debug: Tree::printAllChildsUCTB(gameTree.getRoot());

	NodeType *chosen_node = selectSaveChild(gameTree.getRoot());
	int chosenTurn = chosen_node->chosenTurnThatLeadedToThisNode;
	return chosenTurn;
}

///////////////////////////////////////////////////////////////////////////

int GameAI::pickBestMove(const GamePanel &gp) {
    int col_move;

    // try to look for a winning move
    for (col_move = 0; col_move < gp.getMAX_X(); ++col_move) {
        GamePanel newState = gp;	// make a mutable copy of the const GamePanel
        newState.insertTokenIntoColumn(col_move); // make the move on the copy previously made

        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == AI_Player) {
            return col_move; // the AI should do this move to win!
        }
    }

	// try to look for a move that hinders the opponent from winning
    for (col_move = 0; col_move < gp.getMAX_X(); ++col_move) {
        GamePanel newState = gp;		// make a mutable copy of the const GamePanel
		newState.setTurnPlayer(OP_Player);	// simulate a turn of the opponent, so its his turn now
        newState.insertTokenIntoColumn(col_move); // make the move on the copy previously made

        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == OP_Player) {
			return col_move; // the opponent player will probably make this move to win in his next turn, so hinder him on doing that!
        }
    }

	// here: No winning move found or hindering needed, so pick a random move
    col_move = pickRandomMove(gp);
    return col_move;
}

int GameAI::pickRandomMove(const GamePanel &gp) {
    vector<vector<int>> gameData = gp.getGameData();

    // collect every column, where a token can be put into:
    vector<int> possibleColumns;
    for (int i = 0; i < gp.getMAX_X(); ++i) {
        if (gameData[i][0] == FREE_FIELD) {
            possibleColumns.push_back(i);
        }
    }
	if (possibleColumns.empty()) { return -1; } // all columns are already full!

    /* initialize random seed: */
    time_t t;
    time(&t);
    srand((unsigned int) t);

    /* generate secret number between 1 and MAX_X: */
    int iRand = rand() % possibleColumns.size() + 0;

    int randomColumn = possibleColumns[iRand];
    return randomColumn;
}

int GameAI::doRandomMove(GamePanel &gp) {
    int pickedColumn = pickRandomMove(gp);
    if (pickedColumn != -1) gp.insertTokenIntoColumn(pickedColumn);
    return pickedColumn;
}

void GameAI::expandAllChildrenOf(NodeType *Node) {
    // go through all turns:
    for (int col = 0; col < simulatedGamePanel.getMAX_X(); ++col) {
        int columnChosen = col;
        int col_err = simulatedGamePanel.insertTokenIntoColumn(columnChosen);

        if (col_err != -1) {
            NodeType *newNode = gameTree.createNewNode();
            newNode->UCTB = rand() % 1000 + 10000;
            newNode->chosenTurnThatLeadedToThisNode = columnChosen;
            Tree::addNodeTo(newNode, Node);
        }
    }
}

/////////////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeType * GameAI::recursive_selection(NodeType *Node) {
	NodeType *selectedNode = Node;

	// if current viewed node is not a Leaf Node L,
    if (!(Node->childNodes).empty()) {
		// go through all childNodes and choose the one with the highest UCTB-value
        NodeType *next = nullptr;
        double max_uctb = 0.0f;
        for (size_t i = 0; i < (Node->childNodes).size(); ++i) {
            if (max_uctb <= (Node->childNodes[i])->UCTB) {
                max_uctb = (Node->childNodes[i])->UCTB;
                next = (Node->childNodes[i]);
            }
        }
		// replay what has been already analysed inside the game tree
        simulatedGamePanel.insertTokenIntoColumn(next->chosenTurnThatLeadedToThisNode);

		// ::: recursive call :::
		// select node with highest UCTB value all the way down to a leaf-node
		selectedNode = recursive_selection(next);	
    }

	// leaf node L reached (termination condition reached)
    return selectedNode;
}


/////////////////////////////////
/// E X P A N S I O N ///
/////////////////////////
NodeType * GameAI::expansion(NodeType *leaf_node)
{
    // e1.) Does the Leaf Node L node end the Game? (won/loss/tie)?
    Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
    if (hasWonSim == PLAYER_1 || 
		hasWonSim == PLAYER_2 || 
		hasWonSim == BOTH) {
		return leaf_node;  // if so, don't expand!
	}

    // sim1.) choose a good move (columnChosen)
    int columnChosen = pickBestMove(simulatedGamePanel);
    if (columnChosen == -1) {
        return leaf_node;  // if game panel is full => FULL_TIE and stop simulating
    }
    // sim2.) do the selected move (columnChosen)
    simulatedGamePanel.insertTokenIntoColumn(columnChosen);

    // e2.) expansion: create a newNode
    NodeType *newNodeC = gameTree.createNewNode();
    newNodeC->chosenTurnThatLeadedToThisNode = columnChosen;
    newNodeC->UCTB = rand() % 1000 + 10000;	// assign unvisited nodes with a very large UCTB-value

    // e4.) add the newly created Node to the leaf node L
    Tree::addNodeTo(newNodeC, leaf_node);
    return newNodeC; // = expanded_node C
}

 /////////////////////////////////
 /// S I M U L A T I O N ///
 ///////////////////////////
double GameAI::simulation(NodeType *expanded_node) {
    // has someone won?
    Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
    if (hasWonSim == AI_Player) {
        return VALUE_WIN; // AI has won immediately => stop simulating
    } else if (hasWonSim == OP_Player) {
        return VALUE_LOOSE; // OP has won immediately => stop simulating
    }

    // for (int i = 0; i < NUM_OF_SIMULATION_FRAMES; ++i)
    while (1) {
        // GamePanel::drawGamePanelOnConsole(simulatedGamePanel.getGameData(), simulatedGamePanel.getMAX_X(), simulatedGamePanel.getMAX_Y());
        // system("PAUSE");

        // choose a move
        int columnChosen = pickBestMove(simulatedGamePanel);
        if (columnChosen == -1) {
            return VALUE_DRAW; // if game panel is full => FULL_TIE and stop simulating
        }
        // sim2.) chose a move
        simulatedGamePanel.insertTokenIntoColumn(columnChosen);
			   
        // has someone won?
        Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
        if (hasWonSim == AI_Player) {
            return VALUE_WIN; // AI has won => stop simulating
        } else if (hasWonSim == OP_Player) {
            return VALUE_LOOSE; // OP has won => stop simulating
        }
    }
    return VALUE_DRAW;
}

///////////////////////////////////////
/// B A C K P O P A G A T I O N ///
///////////////////////////////////
void GameAI::backpropagation(NodeType *expanded_node, double ratingToBeUpdated) {
    // GamePanel::drawGamePanelOnConsole(gameDataH, actualGamePanel.getMAX_X(), actualGamePanel.getMAX_Y());

	// b1.) backpropagate from expandednode (C) to Leaf Node (L) and other nodes on that way up to root node (R)
    NodeType *cur_node = expanded_node;
    do {
        // b2.) increase the amount of visits of each node and add that to the rating
        cur_node->value += ratingToBeUpdated;
		++cur_node->visits;

    } while ((cur_node = cur_node->parent) != nullptr);

    // b3.) update the rating (UCTS) of the nodes up to the root (exluding the root)
    cur_node = expanded_node;
    do {
        //-------------------------------------------------------------------
        if (cur_node->visits != 0) {
            /*
				constant C = curiousity of the algorithm
				small C => game tree gets deeper expanded (only the best variation gets explored)
				big C => game tree gets broader expanded (nodes with lesser visits are prefered)
			*/
       
            // ::: exploitation :::
            double winratio = cur_node->value / cur_node->visits;

            // ::: exploration :::
            double uct = CURIOUSITY_FACTOR * sqrt(log(cur_node->parent->visits) / cur_node->visits);

            cur_node->UCTB = winratio + uct;
        }
        //-------------------------------------------------------------------
        cur_node = cur_node->parent;
    } while (cur_node->parent != NULL);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

NodeType * GameAI::selectSaveChild(NodeType *Node) {
    NodeType *selected_node;
    int A_PARAM = 4;
    double max_save_val = 0.0;

    for (size_t i = 0; i < Node->childNodes.size(); ++i) // for all children
    {
        // inspect all siblings...
        NodeType *next = Node->childNodes[i];
        if (next->visits <= 0) continue;

        double save_val;
        save_val = (next->value / next->visits) + (A_PARAM / sqrt(next->visits));

        if (max_save_val <= save_val) {
            max_save_val = save_val;
			selected_node = next;
        }
    }
    return selected_node;

}

NodeType * GameAI::selectRobustChild(NodeType *Node) {
    NodeType *selected_node;
    int max_visit_count = 0;

    // von node aus alle kinder:
    for (size_t i = 0; i < Node->childNodes.size(); ++i) // for all children
    {
        // inspect all siblings
        NodeType *next = Node->childNodes[i];

        double save_val;
        save_val = next->visits;

        if (max_visit_count <= save_val) {
            max_visit_count = save_val;
			selected_node = next;
        }
    }
    return selected_node;
}

