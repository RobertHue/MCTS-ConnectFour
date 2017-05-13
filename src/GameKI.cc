
#include "GameKI.h"

/**
 * trial and error each column whether a placement will lead to a win for the turnPlayer
 * falls kein win-zug oder hindern des gegner mglich => whle einen zuflligen Zug aus!
 * wichtig: diese Funktion ndert das bergebene Spielfeld nicht (const)
 */
int GameKI::pickBestColMove(const GamePanel &gp) {
    int col_move;
    // Try moves from maximum first looking for a winning move
    for (col_move = 0; col_move < gp.getMAX_X(); ++col_move) {
        // Don't want to change current state, so make copy
        GamePanel newState = gp;

        newState.insertTokenIntoColumn(col_move); // Make the move on the copy made


        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == newState.getOtherPlayer()) {
            return col_move; // next Turn Player will probably make this move to win
        }
    }
    // Try moves from maximum first looking for a winning move
    for (col_move = 0; col_move < gp.getMAX_X(); ++col_move) {
        // Don't want to change current state, so make copy
        GamePanel newState = gp;
        newState.nextTurn();

        newState.insertTokenIntoColumn(col_move); // Make the move on the copy made

        Player playerThatJustWon = newState.hasSomeoneWon();
        if (playerThatJustWon == newState.getOtherPlayer()) {
            return col_move; // next Turn Player will probably make this move to win
        }
    }

    col_move = pickRandomTurn(gp);
    return col_move; // No winning move found
}

/*
 * pick a random turn in column col
 * return: the column col (0-7)
 *		returns -1 when every column is already full
 */
int GameKI::pickRandomTurn(const GamePanel &gp) {
    vector<vector<int>> gameData = gp.getGameData();

    // ermittle alle mglichen Spalten in denen eine Mnze geworfen werden kann
    vector<int> possibleColumns;
    for (int i = 0; i < gp.getMAX_X(); ++i) {
        if (gameData[i][0] == FREE_FIELD) {
            possibleColumns.push_back(i);
        }
    }
    if (possibleColumns.empty()) return -1; // alle columns breits voll!



    /* initialize random seed: */
    time_t t;
    time(&t);
    srand((unsigned int) t);

    /* generate secret number between 1 and MAX_X: */
    int iRand = rand() % possibleColumns.size() + 0;

    int col = possibleColumns[iRand];
    return col;
}
// macht einen zuflligen spielzug und ndert gameDataH
// es wird die position des Spielsteins zurckgegeben

int GameKI::doRandomTurnH(GamePanel &gp) {
    int col = pickRandomTurn(gp);
    if (col != -1) gp.insertTokenIntoColumn(col);
    return col;
}

/*
Expansion aller child nodes vom root node aus:
 */
void GameKI::expandAllChildrenOf(t_node *node) {
    // alle Zge durchgehen:
    for (int col = 0; col < simulatedGamePanel.getMAX_X(); ++col) {
        int columnChosen = col;
        int col_err = simulatedGamePanel.insertTokenIntoColumn(columnChosen);

        if (col_err != -1) {
            t_node *newNode = gameTree.createNewNode();
            newNode->UCTB = rand() % 1000 + 10000;
            newNode->chosenTurnThatLeadedToThisNode = columnChosen;
            Tree::addNodeTo(newNode, node);
        }
    }
}

t_node * GameKI::UCTSelect(t_node *node) {
    t_node *res = node;
    double best_uct = 0;

    for (size_t i = 0; i < node->childNodes.size(); ++i) // for !all! children
    {
        // inspect all siblings...
        t_node *next = node->childNodes[i];



        double uctvalue = next->UCTB;

        if (next->visits > 0) {
            double winrate = next->value / next->visits;
            double uct = C_FACTOR * sqrt(log(node->visits) / next->visits);
            uctvalue = winrate + uct;
        } else {
            // Always play a random unexplored move first
            uctvalue = 10000 + 1000 * rand();
        }



        if (best_uct < uctvalue) { // get max uctvalue of all children
            best_uct = uctvalue;
            i = 0;
            node = next;
            res = next;
        }
    }

    stack<int> stack_turns;

    while (node->parent != NULL) {
        stack_turns.push(node->chosenTurnThatLeadedToThisNode);
        node = node->parent;
    }

    while (!stack_turns.empty()) {
        //simuliertes Spiel anpassen!!!
        int value = stack_turns.top();
        simulatedGamePanel.insertTokenIntoColumn(value);
        stack_turns.pop();
    }

    return res;
}

t_node * GameKI::recursive_selection(t_node *node) {
    // Abbruchbedingung: (Blatt erreicht)
    if ((node->childNodes).empty()) {
        return node;
    } else {
        // dursuche alle Nachfolger und whle denjenigen mit dem hchsten UCTB aus 
        t_node *next = NULL;
        double max_uctb = 0.0f;
        for (size_t i = 0; i < (node->childNodes).size(); ++i) {
            if (max_uctb <= (node->childNodes[i])->UCTB) {
                max_uctb = (node->childNodes[i])->UCTB;
                next = (node->childNodes[i]);
            }
        }
        simulatedGamePanel.insertTokenIntoColumn(next->chosenTurnThatLeadedToThisNode);
        recursive_selection(next);
    }
    return nullptr;
}

/*
Walk down the game tree using a selection function at each
node to determine which child node to walk to next.
Stop when you reach a node that you dont have statistics for yet.
 */
t_node * GameKI::selection(t_node *node) // aka follow_confident_branch()
{
    /*
    // sel1.) whle unbesuchte Nodes aus
    if (node->visits == 0)	return node;

    // sel2.) whle unbesuchte Nodes der Nachfolger dieses Nodes
    for (int i = 0; i < (node->childNodes).size(); ++i)
    {
    if ((node->childNodes[i])->visits == 0) return node->childNodes[i];
    }
     */

    // sel3.) selektiere Knoten mit dem hchsten UCTB-Wert und gehe so weiter bis zu einem Blatt
    // Falls alle Nachfolger bereits einmal besucht wurden, whle den Node mit der hchsten UCTB
    t_node *selected_node = node;
    do {
        // dursuche alle Nachfolger... 
        double max_uctb = 0.0f;
        for (size_t i = 0; i < (node->childNodes).size(); ++i) {
            if (max_uctb <= (node->childNodes[i])->UCTB) {
                max_uctb = (node->childNodes[i])->UCTB;
                selected_node = (node->childNodes[i]);
            }
        }
        node = selected_node;

        //********************************
        // simuliertes Spiel anpassen!!!
        //********************************
        simulatedGamePanel.insertTokenIntoColumn(node->chosenTurnThatLeadedToThisNode);

    } while (!node->childNodes.empty()); // falls ein blatt erreicht wird!


    return selected_node; // = Leaf Node L
}

t_node * GameKI::expansion(t_node *leaf_node) // http://de.slideshare.net/ftgaic/mcts-ai
{
    // e1.) Does the Leaf Node L node ends the Game? Gewonnen oder verloren...? 
    Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
    if (hasWonSim == PLAYER_1 || hasWonSim == PLAYER_2) return leaf_node; // jemand hat gewonnen => expandiere nicht!


    // sim1.) whle einen geeigneten Zug aus (zufllig/deterministisch)
    int columnChosen = pickBestColMove(simulatedGamePanel);
    if (columnChosen == -1) {
        return leaf_node; // falls spielfeld voll & unentschieden => hre auf zu simulieren
    }
    // sim2.) fhre den gewhlten Zug aus (columnChosen)
    simulatedGamePanel.insertTokenIntoColumn(columnChosen);

    //int columnChosen = doRandomTurnH(simulatedGamePanel);
    //if (columnChosen == -1) { return leaf_node; }	// keine Zge mehr mglich => Unentschieden!


    // e2.) expansion: create a newNode
    t_node *newNode = gameTree.createNewNode();
    newNode->chosenTurnThatLeadedToThisNode = columnChosen;
    newNode->parent = leaf_node;
    newNode->UCTB = rand() % 1000 + 10000;

    // e4.) add the newly created Node it to the leaf node L
    Tree::addNodeTo(newNode, leaf_node);
    return newNode; // = expanded_node C
}

/*
 * zuf�lliges Spiel von ausgew�hlten Knoten expanded_node (C) simulieren (H)
 * nicht die simulierten Knoten einh�ngen !!!! Tree::addNodeTo(newNode, expanded_node);
 * die Simulation dient nur dazu, den expanded node und pfad dorthin zu bewerten!!!
 * returns a double indicaing if the game was won by the AI
 * 1 - the game was won by the ai
 * 0 - the game was lost by the ai
 * 0.5 - the game was a draw
 * rewards taken from http://www.tantrix.com/Tantrix/TRobot/MCTS%20Final%20Report.pdf
 */
double GameKI::simulation(t_node *expanded_node) {
    // hat jemand gewonnen?
    Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
    if (hasWonSim == AI_Player) {
        return VALUE_WIN_IMM; // KI hat gewonnen => h�re auf zu simulieren
    } else if (hasWonSim == OP_Player) {
        return VALUE_LOOSE_IMM; // opp hat gewonnen => h�re auf zu simulieren
    }

    // for (int i = 0; i < NUM_OF_SIMULATION_FRAMES; ++i)
    while (1) {
        // GamePanel::drawGamePanelOnConsole(simulatedGamePanel.getGameData(), simulatedGamePanel.getMAX_X(), simulatedGamePanel.getMAX_Y());
        // system("PAUSE");


        // w�hle einen geeigneten Zug aus (zuf�llig/deterministisch)
        int columnChosen = pickBestColMove(simulatedGamePanel);
        if (columnChosen == -1) {
            return VALUE_FULL_TIE; // falls spielfeld voll & unentschieden => h�re auf zu simulieren
        }
        // sim2.) f�hre den gew�hlten Zug aus (columnChosen)
        simulatedGamePanel.insertTokenIntoColumn(columnChosen);



        // hat jemand gewonnen?
        Player hasWonSim = simulatedGamePanel.hasSomeoneWon();
        if (hasWonSim == AI_Player) {
            return VALUE_WIN; // KI hat gewonnen => h�re auf zu simulieren
        } else if (hasWonSim == OP_Player) {
            return VALUE_LOOSE; // opp hat gewonnen => h�re auf zu simulieren
        }
    }
    return VALUE_TIE;
}

void GameKI::backpropagation(t_node *expanded_node, double bewertung) {
    // GamePanel::drawGamePanelOnConsole(gameDataH, actualGamePanel.getMAX_X(), actualGamePanel.getMAX_Y());

    // b1.) gehe von expanded_node C aus �ber L und weitere Knoten hoch zur Root
    t_node *cur_node = expanded_node;
    do {
        // b2.) erh�he die Anzahl der Besuche der Knoten und addiere die Bewertung zu/von den Knoten
        cur_node->value += bewertung;
        cur_node->visits++;

    } while ((cur_node = cur_node->parent) != NULL);

    // b3.) update die Bewertung der Knoten bis zur Wurzel (ausschlie�lich)
    cur_node = expanded_node;
    do {
        //-------------------------------------------------------------------
        if (cur_node->visits != 0) {
            /*
            Konstante C = Neugierde des Algorithmus
            kleines C => Baum wird tiefer expandiert (nur die beste Variante wird untersucht)
            gro�es C => Baum wird breiter expandiert (weniger oft besuchte Knoten werden bevorzugt)
             */
            // ::: exploitation :::
            double winrate = cur_node->value / cur_node->visits;

            // ::: exploration :::
            double uct = C_FACTOR * sqrt(log(cur_node->parent->visits) / cur_node->visits);

            cur_node->UCTB = winrate + uct;
        }
        //-------------------------------------------------------------------
        cur_node = cur_node->parent;
    } while (cur_node->parent != NULL);
}

t_node * GameKI::selectSaveChild(t_node *node) {
    t_node *sel_node;
    int A_PARAM = 4;
    double max_save_val = 0.0;

    // von node aus alle kinder:
    for (size_t i = 0; i < node->childNodes.size(); ++i) // for !all! children
    {
        // inspect all siblings...
        t_node *next = node->childNodes[i];
        if (next->visits <= 0) continue;

        double save_val;
        save_val = (next->value / next->visits) + (A_PARAM / sqrt(next->visits));

        if (max_save_val <= save_val) {
            max_save_val = save_val;
            sel_node = next;
        }
    }
    return sel_node;

}

t_node * GameKI::selectRobustChild(t_node *node) {
    t_node *sel_node;
    // int A_PARAM = 4; TODO is unused
    int max_visit_count = 0;

    // von node aus alle kinder:
    for (size_t i = 0; i < node->childNodes.size(); ++i) // for !all! children
    {
        // inspect all siblings...
        t_node *next = node->childNodes[i];

        double save_val;
        save_val = next->visits;

        if (max_visit_count <= save_val) {
            max_visit_count = save_val;
            sel_node = next;
        }
    }
    return sel_node;

}

GameKI::GameKI(Player tokenKI) : AI_Player(tokenKI) {
    if (AI_Player == PLAYER_1)
        OP_Player = PLAYER_2;
    else
        OP_Player = PLAYER_1;
}

// weise allen noch unbesuchten nodes einen sehr groen zuflligen UCTB wert zu(uctvalue = 10000 + 1000 * rand();)
// pre: wird erst ausgefhrt, nachdem der Gegner seinen Zug gettigt hat:
// post: KI hat seinen nchsten Zug bestimmt
// return: spalte, in der der nchste Zug gesetzt wird

int GameKI::calculateNextTurn(const GamePanel &gPanel) {
    gameTree = Tree();

    cout << "Start des MCTS!" << endl;
    t_node *selected_node;
    t_node *expanded_node;
    double bewertung;

    simulatedGamePanel = gPanel; // = echte tiefe Kopie
    expandAllChildrenOf(gameTree.getRoot());
    for (size_t i = 0; i < MAX_NUM_OF_ITERATIONS; ++i) {
        // reset the simulated GamePanel
        simulatedGamePanel = gPanel; // = echte tiefe Kopie
        // Annahme: root-Node ist immer nach dem Spielzug des Gegners (OP_Player)

        ///////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////
        selected_node = recursive_selection(gameTree.getRoot());
        expanded_node = expansion(selected_node);
        bewertung = simulation(expanded_node);
        backpropagation(expanded_node, bewertung);
        ///////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////

        // debug: 
        // Tree::levelOrder(gameTree.getRoot()); 
        // system("PAUSE");
        // GamePanel::drawGamePanelOnConsole(simulatedGamePanel.getGameData(), simulatedGamePanel.getMAX_X(), simulatedGamePanel.getMAX_Y());
        // system("PAUSE");
        // cout << "X: " << simulatedGamePanel.getPositionOfLastPlacedToken().x << "\nY: " << simulatedGamePanel.getPositionOfLastPlacedToken().y << endl;

    }
    cout << "Ende des MCTS!" << endl;
    // debug: Tree::printAllChildsUCTB(gameTree.getRoot());

    t_node *chosen_node = selectSaveChild(gameTree.getRoot());
    int chosenTurn = chosen_node->chosenTurnThatLeadedToThisNode;
    return chosenTurn;
}
