#include <chrono>   // for high_resolution_clock
#include <iterator> // for std::advance
#include <limits>   // for numeric::limits to get the smallest double
#include <memory>
#include <stdexcept>

#include "ai/GameAI.h"
#include "ai/Tree.h"

std::ostream &operator<<(std::ostream &os, const NodeDataType &nodeData)
{
    os << nodeData.sequenceThatLeadedToThisNode
       << "::" << static_cast<int>(nodeData.UCTB) << " | " << nodeData.rating
       << "/" << nodeData.visits << '\t';

    return os; // enables concatenation of ostreams with <<
}

inline void printChildNodeInfo(const NodeTypePtr node)
{
    const int firstTab = 15;
    const int secondTab = 8;
    std::cout << "-----------------------\n";
    std::cout << "Child Node Info:\n";
    for (auto &n : node->childNodes)
    {
        std::cout << "[ Move:" << n->data.chosenMoveThatLeadedToThisNode
                  << std::setw(firstTab) << "Wins:" << std::setw(secondTab)
                  << n->data.rating << std::setw(firstTab)
                  << "Visits:" << std::setw(secondTab) << n->data.visits
                  << std::setw(firstTab) << "UCTB:" << std::setw(secondTab)
                  << n->data.UCTB << std::setw(firstTab)
                  << "winratio:" << std::setw(secondTab) << n->data.winratio
                  << std::setw(firstTab) << "uct:" << std::setw(secondTab)
                  << n->data.uct << "]\n";
    }
    std::cout << "-----------------------\n";
}

GameAI::GameAI(Player tokenKI) : AI_Player(tokenKI), m_pGameTree(new TreeType())
{
    if (AI_Player == Player::PLAYER_1)
        OP_Player = Player::PLAYER_2;
    else
        OP_Player = Player::PLAYER_1;
}

int GameAI::findNextMove(const GameState &gameState)
{
    m_pGameTree = std::make_unique<TreeType>();
    // set root of the game tree to be owned by the opponent player
    m_pGameTree->getRoot()->data.player = OP_Player;

    std::cout << "Start of MCTS!\n";

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    // make a mutable copy of the const GameState (deep copy)
    simulatedGameState = gameState;
    // expand all adjacent child nodes at once from the root node:
    auto root_node = m_pGameTree->getRoot();
    expandAllChildrenOf(root_node);

    for (size_t i = 0; i < MAX_NUM_OF_ITERATIONS; ++i)
    {
        simulatedGameState = gameState; // reset the simulated GameState
        const NodeTypePtr root_node = m_pGameTree->getRoot();
        const NodeTypePtr selected_node = selectPromisingNode(root_node);
        const NodeTypePtr expanded_node = expandNode(selected_node);
        const Value rating = simulation(expanded_node);
        backpropagation(expanded_node, rating);
    }

    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed = finish - start;

    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "End of MCTS!\n";
    const NodeTypePtr chosenNode =
        selectMostVisitedChild(m_pGameTree->getRoot());
    if (chosenNode == nullptr)
    {
        std::cout << "AI could not choose a column. Possible reason: board is "
                     "full.\n";
        return -1;
    }
    const int chosenMove = chosenNode->data.chosenMoveThatLeadedToThisNode;
    std::cout << "AI has chosen the column : " << chosenMove << "\n";
    return chosenMove;
}


void GameAI::expandAllChildrenOf(NodeTypePtr nodeToExpand)
{
    const std::vector<int> possibleMoves = setupPossibleMovesOf(nodeToExpand);

    const Player currentPlayer = (nodeToExpand->data.player == Player::PLAYER_1)
                                     ? Player::PLAYER_2
                                     : Player::PLAYER_1;

    for (const int pmove : possibleMoves)
    {
        // update the game TreeType to hold expanded nodes
        const NodeTypePtr newNode = m_pGameTree->createNewNode(nodeToExpand);
        newNode->data.UCTB = rand() % 1000 + 10000;
        newNode->data.chosenMoveThatLeadedToThisNode = pmove;
        newNode->data.sequenceThatLeadedToThisNode =
            nodeToExpand->data.sequenceThatLeadedToThisNode + "." +
            std::to_string(newNode->data.chosenMoveThatLeadedToThisNode);
        newNode->data.level = nodeToExpand->data.level + 1;
        newNode->data.player = currentPlayer;
    }
}

NodeTypePtr GameAI::findBestNodeWithUCT(NodeTypePtr node)
{
    // go through all childNodes and choose the one with the highest UCTB
    NodeTypePtr nodeWithHighestUCT =
        nullptr; // is possible when childNodes are empty

    double max_uctb = -std::numeric_limits<double>::max();
    for (auto &n : node->childNodes)
    {
        const double curUCTB = uctValue(n);
        if (max_uctb <= curUCTB)
        {
            max_uctb = curUCTB;
            nodeWithHighestUCT = n;
        }
    }
    return nodeWithHighestUCT;
}

double GameAI::uctValue(NodeTypePtr node)
{
    // protect against division by zero
    if (node->data.visits == 0)
    {
        return std::numeric_limits<double>::max();
    }

    // ::: exploitation :::
    node->data.winratio = node->data.rating / node->data.visits;

    // ::: exploration :::
    node->data.uct = CURIOUSITY_FACTOR *
                     sqrt(static_cast<double>(log(node->parent->data.visits)) /
                          static_cast<double>(node->data.visits));

    // calculate UCTB value
    node->data.UCTB = node->data.winratio + node->data.uct;
    return node->data.UCTB;
}

/////////////////////////
/// S E L E C T I O N ///
/////////////////////////
NodeTypePtr GameAI::selectPromisingNode(NodeTypePtr rootNode)
{
    auto selectedNode = std::move(rootNode);

    while (!(selectedNode->childNodes.empty()))
    {
        const NodeTypePtr tmpNode = selectedNode;
        selectedNode = findBestNodeWithUCT(selectedNode);

        if (selectedNode == nullptr)
        {
            throw std::invalid_argument("nodeTemp is nullptr");
        }

        // replay what has been already analysed inside the game TreeType
        simulatedGameState.insertTokenIntoColumn(
            selectedNode->data.chosenMoveThatLeadedToThisNode);

        // if a level node having 10 visits at depth level of 1,
        // then expand all of its child nodes
        if ((tmpNode->data.visits) == EXPAND_FULLY_ON_VISITS &&
            tmpNode->data.level >= 1)
        {
            expandAllChildrenOf(tmpNode);
        }
    }

    return selectedNode;
}

/////////////////////////
/// E X P A N S I O N ///
/////////////////////////	// L -> C
NodeTypePtr GameAI::expandNode(NodeTypePtr leaf_node)
{
    // e1.) Does the Leaf Node L node end the Game? (won/loss/tie)?
    const Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == Player::PLAYER_1 || hasWonSim == Player::PLAYER_2 ||
        hasWonSim == Player::DRAW)
    {
        return leaf_node; // if so, don't expand!
    }

    //////////////////////////////////////

    std::vector<int> possibleMoves = setupPossibleMovesOf(leaf_node);

    // sim1.) choose a good move from the movePossibilites (columnChosen)
    // int columnChosen = pickRandomMoveFrom(possibleMoves, simulatedGameState);
    const int columnChosen =
        pickBestMoveFrom(possibleMoves, simulatedGameState);
    if (columnChosen == -1)
    {
        return leaf_node; //if game panel is full => FULL_TIE & stop sim
    }

    // sim2.) do the selected move (columnChosen)
    const Player currentPlayer = simulatedGameState.getTurnPlayer();
    simulatedGameState.insertTokenIntoColumn(columnChosen);

    // e2.) expansion: create a newNode
    NodeTypePtr newNodeC = m_pGameTree->createNewNode(leaf_node);
    newNodeC->data.UCTB = rand() % 1000 + 10000;
    newNodeC->data.chosenMoveThatLeadedToThisNode = columnChosen;
    newNodeC->data.sequenceThatLeadedToThisNode =
        leaf_node->data.sequenceThatLeadedToThisNode + "." +
        std::to_string(columnChosen);
    newNodeC->data.level = leaf_node->data.level + 1;
    newNodeC->data.player = currentPlayer;

    return newNodeC; // return expanded_node C
}

///////////////////////////
/// S I M U L A T I O N ///
///////////////////////////
double GameAI::simulation(NodeTypePtr expanded_node)
{
    // has someone won?
    const Player hasWonSim = simulatedGameState.hasSomeoneWon();
    if (hasWonSim == AI_Player)
    {
        return Value::WIN; // AI has won immediately => stop simulating
    }
    else if (hasWonSim == OP_Player)
    {
        return Value::LOOSE; // OP has won immediately => stop simulating
    }
    else if (hasWonSim == Player::DRAW)
    {
        return Value::DRAW;
    }

    // for (int i = 0; i < NUM_OF_SIMULATION_FRAMES; ++i)
    while (true)
    {
        // choose a move
        // int columnChosen = pickRandomMove(simulatedGameState);
        const int columnChosen = pickBestMove(simulatedGameState);
        if (columnChosen == -1)
        {
            return Value::DRAW; // if game panel is full => FULL_TIE & stop sim
        }
        // sim2.) chose a move
        simulatedGameState.insertTokenIntoColumn(columnChosen);

        // has someone won?
        const Player hasWonSim = simulatedGameState.hasSomeoneWon();
        if (hasWonSim == AI_Player)
        {
            return Value::WIN; // AI has won => stop simulating
        }
        else if (hasWonSim == OP_Player)
        {
            return Value::LOOSE; // OP has won => stop simulating
        }
        else if (hasWonSim == Player::DRAW)
        {
            return Value::DRAW;
        }
    }
    return Value::DRAW;
}

/////////////////////////////////////
/// B A C K P R O P A G A T I O N ///
/////////////////////////////////////
void GameAI::backpropagation(NodeTypePtr expanded_node,
                             const Value ratingToBeUpdated)
{
    // if rating is a draw then add it to every node upwards
    const bool switchSameRating =
        (ratingToBeUpdated == Value::DRAW) ? true : false;

    Value ratingToBeUpdatedAI = Value::DRAW;
    Value ratingToBeUpdatedOther = Value::DRAW;

    if (!switchSameRating)
    {
        ratingToBeUpdatedAI = ratingToBeUpdated;
        ratingToBeUpdatedOther =
            (ratingToBeUpdated == Value::WIN) ? Value::LOOSE : Value::WIN;
    }

    ///////////////////////////////////////////////////////////////////////////
    //update visit and rating values upwards from expanded node C to L to root
    NodeTypePtr cur_node = std::move(expanded_node);
    while (cur_node != nullptr)
    {
        if (switchSameRating)
        {
            cur_node->data.rating += ratingToBeUpdated;
        }
        else
        { // ratingToBeUpdated != Value::DRAW
            if (cur_node->data.player == this->AI_Player)
            {
                cur_node->data.rating += ratingToBeUpdatedAI;
            }
            else if (cur_node->data.player == this->OP_Player)
            {
                cur_node->data.rating += ratingToBeUpdatedOther;
            }
        }

        //-------------------------------------------------------------------
        ++(cur_node->data.visits);   // increase the visits of every node
        cur_node = cur_node->parent; // advance upwards
    }
}

NodeTypePtr GameAI::selectMostVisitedChild(NodeTypePtr rootNode)
{
    NodeTypePtr selected_node = nullptr;
    std::size_t max_visit_count = 0;

    // inspect all children visits and store node with max visits
    for (const auto &child : rootNode->childNodes) //for all children
    {
        if (max_visit_count <= child->data.visits)
        {
            max_visit_count = child->data.visits;
            selected_node = child;
        }
    }
    return selected_node;
}

///////////////////////////////////////////////////////////////////////////

int GameAI::pickBestMoveFrom(std::vector<int> &possibleMoves,
                             const GameState &gs)
{
    const Player currentPlayer = gs.getTurnPlayer();
    const Player OtherPlayer = gs.getOtherPlayer();

    // try to look for a winning move for current player
    for (const auto &move : possibleMoves)
    {
        //GameState tmpState = gs;  // make a mutable copy of the const GameState
        //tmpState.insertTokenIntoColumn(possibleMoves[i]); // make the move on the copy

        const Player playerThatJustWon = gs.wouldSomeoneWin(move);
        if (playerThatJustWon == currentPlayer)
        {
            return move; // the currentPlayer should do this move to win!
        }
    }

    // try to look for a move that hinders the OtherPlayer from winning
    for (const auto &move : possibleMoves)
    {
        const Player playerThatJustWon = gs.wouldSomeoneWin(move);
        if (playerThatJustWon == OtherPlayer)
        {
            return move;
            // OtherPlayer will probably make this move to win in his turn,
            // so hinder him on doing that!
        }
    }

    // here: No winning move found or hindering needed, so pick a random move
    return pickRandomMoveFrom(possibleMoves, gs);
}

int GameAI::pickRandomMoveFrom(std::vector<int> &possibleMoves,
                               const GameState &gs)
{
    /* initialize random seed: */
    time_t t = std::time(nullptr);
    time(&t);
    srand((unsigned int)t);

    /* generate secret number between 1 and MAX_X: */
    const auto iRand = rand() % possibleMoves.size() + 0;

    const int randomColumn = possibleMoves[iRand];
    return randomColumn;
}

int GameAI::pickBestMove(const GameState &gs)
{
    int col_move = 0;
    const Player currentPlayer = gs.getTurnPlayer();
    const Player OtherPlayer = gs.getOtherPlayer();

    // try to look for a winning move for current player
    for (col_move = 0; col_move < gs.getMAX_X(); ++col_move)
    {
        const Player playerThatJustWon = gs.wouldSomeoneWin(col_move);
        if (playerThatJustWon == currentPlayer)
        {
            return col_move; // the currentPlayer should do this move to win!
        }
    }

    // try to look for a move that hinders the OtherPlayer from winning
    for (col_move = 0; col_move < gs.getMAX_X(); ++col_move)
    {
        const Player playerThatJustWon = gs.wouldSomeoneWin(col_move);
        if (playerThatJustWon == OtherPlayer)
        {
            return col_move;
            // OtherPlayer will probably make this move to win in his turn,
            // so hinder him on doing that!
        }
    }

    // here: No winning move found or hindering needed, so pick a random move
    col_move = pickRandomMove(gs);
    return col_move;
}

int GameAI::pickRandomMove(const GameState &gs)
{
    std::vector<std::vector<int>> gameData = gs.getGameData();

    // collect every column, where a token can be put into:
    std::vector<int> possibleColumns;
    for (int i = 0; i < gs.getMAX_X(); ++i)
    {
        if (gameData[i][0] == static_cast<int>(Player::NONE))
        {
            possibleColumns.push_back(i);
        }
    }
    if (possibleColumns.empty())
    {
        return -1;
    } // all columns are already full

    /* initialize random seed: */
    time_t t = std::time(nullptr);
    time(&t);
    srand((unsigned int)t);

    /* generate secret number between 1 and MAX_X: */
    const auto iRand = rand() % possibleColumns.size() + 0;

    const int randomColumn = possibleColumns[iRand];
    return randomColumn;
}

NodeTypePtr GameAI::pickBestChild(NodeTypePtr node, const GameState &gs)
{
    const int columnChosen = pickBestMove(gs);
    if (columnChosen == -1)
    {
        return pickRandomChild(
            node); // if game panel is full => pick random child
    }

    NodeTypePtr bestChild = nullptr;
    for (auto &n : node->childNodes)
    {
        if (n->data.chosenMoveThatLeadedToThisNode == columnChosen)
        {
            bestChild = n;
        }
    }

    if (bestChild == nullptr)
    {
        return pickRandomChild(node);
    }
    return bestChild;
}

NodeTypePtr GameAI::pickRandomChild(NodeTypePtr node)
{
    if (node->childNodes.size() > 0)
    {
        /* initialize random seed: */
        time_t t = std::time(nullptr);
        time(&t);
        srand((unsigned int)t);

        /* generate secret number between 1 and MAX_X: */
        const auto iRand = rand() % node->childNodes.size() + 0;

        NodeTypePtr randomNode = node->childNodes[iRand];
        return randomNode;
    }
    return nullptr;
}

int GameAI::doRandomMove(GameState &gs)
{
    const int pickedColumn = pickRandomMove(gs);
    if (pickedColumn != -1)
    {
        gs.insertTokenIntoColumn(pickedColumn);
    }
    return pickedColumn;
}

std::vector<int> GameAI::setupPossibleMovesOf(NodeTypePtr node)
{
    // make a deep copy of the simulatedGameState
    const GameState tmpState = simulatedGameState;

    // setup the possible moves:
    std::vector<int> possibleMovesToExpand = tmpState.getPossibleMoves();
    for (auto &n : node->childNodes)
    {
        auto citer = possibleMovesToExpand.cbegin();
        for (auto &p : possibleMovesToExpand)
        {
            if (p == n->data.chosenMoveThatLeadedToThisNode)
            {
                possibleMovesToExpand.erase(citer);
                break; // break out of possibleMovesToExpand loop
            }
            ++citer;
        }
    }
    return possibleMovesToExpand;
}

///////////////////////////////////////////////////////////////////////////

const TreeType &GameAI::getGameTree() const
{
    const TreeType &constRef = *m_pGameTree;
    return constRef;
}

Player GameAI::getPlayer() const
{
    return AI_Player;
}

///////////////////////////////////////////////////////////////////////////
