#define BOOST_TEST_MODULE "TestMCTSModule"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "GameAI.h"
#include "GameState.h"
#include "Tree.h"

BOOST_AUTO_TEST_CASE(TreeTest00)
{
    BOOST_TEST_MESSAGE("TreeTest00");

    const Tree<int> tree;

    //BOOST_CHECK(tree.size() == tree2.size());
}


BOOST_AUTO_TEST_CASE(TestGameState)
{
    bool isMoveValid = false;
    // player_1 = O
    // player_2 = X
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);

    BOOST_CHECK(gs.getMAX_X() == MAX_X);
    BOOST_CHECK(gs.getMAX_Y() == MAX_Y);
    BOOST_CHECK(gs.getNumOfFreeFields() == MAX_X * MAX_Y);

    GameState::GameDataType m_gameData = gs.getGameData();
    for (int i = 0; i < gs.getMAX_X(); ++i)
    {
        for (int j = 0; j < gs.getMAX_Y(); ++j)
        {
            BOOST_CHECK(m_gameData[i][j] == static_cast<int>(Player::NONE));
        }
    }
    isMoveValid = gs.insertTokenIntoColumn(6);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(1);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(4);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(2);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(4);
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(2);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(5);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(3);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(3);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(3);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(6);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(5);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(4);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    isMoveValid = gs.insertTokenIntoColumn(6);
    gs.drawGameStateOnConsole();
    BOOST_CHECK(gs.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    BOOST_CHECK(gs.getNumOfFreeFields() == ((MAX_X * MAX_Y) - 14));
    BOOST_CHECK(gs.getTurnPlayer() == Player::PLAYER_1);
    BOOST_CHECK(gs.getOtherPlayer() == Player::PLAYER_2);
    gs.nextTurn();
    BOOST_CHECK(gs.getTurnPlayer() == Player::PLAYER_2);
    BOOST_CHECK(gs.getOtherPlayer() == Player::PLAYER_1);
    gs.setTurnPlayer(Player::PLAYER_1);
    BOOST_CHECK(gs.getTurnPlayer() == Player::PLAYER_1);
    BOOST_CHECK(gs.getOtherPlayer() == Player::PLAYER_2);

    BOOST_CHECK(gs.getPositionOfLastPlacedToken() == Position(6, 2));

    const std::vector<int> possibleMoves = gs.getPossibleMoves();
    int m = 0;
    for (auto &pm : possibleMoves)
    {
        BOOST_CHECK(pm == m++);
    }
    BOOST_CHECK(possibleMoves.size() == MAX_X);

    GameState tmpGS = gs;
    tmpGS.setTurnPlayer(Player::PLAYER_1);
    isMoveValid = tmpGS.insertTokenIntoColumn(4);
    BOOST_CHECK(tmpGS.hasSomeoneWon() == Player::PLAYER_1);
    BOOST_CHECK(isMoveValid == true);

    GameState tmpGS2 = gs;
    tmpGS2.setTurnPlayer(Player::PLAYER_2);
    isMoveValid = tmpGS2.insertTokenIntoColumn(4);
    BOOST_CHECK(tmpGS2.hasSomeoneWon() == Player::PLAYER_2);
    BOOST_CHECK(isMoveValid == true);

    GameState tmpGS4 = gs;
    tmpGS4.setTurnPlayer(Player::PLAYER_2);
    isMoveValid = tmpGS4.insertTokenIntoColumn(3);
    BOOST_CHECK(tmpGS4.hasSomeoneWon() == Player::NONE);
    BOOST_CHECK(isMoveValid == true);

    GameState tmpGS5 = gs;
    tmpGS5.setTurnPlayer(Player::PLAYER_2);
    isMoveValid = tmpGS5.insertTokenIntoColumn(0);
    BOOST_CHECK(tmpGS5.hasSomeoneWon() == Player::PLAYER_2);
    BOOST_CHECK(isMoveValid == true);
}


/// tests whether the nodes child node size is <= MAX_X
void checkChildCountIteratively(NodeType *node, const int MAX_X)
{
    if (node == nullptr)
    {
        return;
    }

    BOOST_CHECK(node->childNodes.size() <= static_cast<std::size_t>(MAX_X));

    /* first delete all subtrees (from left to right) */
    for (std::size_t i = 0; i < node->childNodes.size(); ++i)
    {
        checkChildCountIteratively(node->childNodes[i], MAX_X);
    }
}


void checkChildVisitsValuePlausibily(const GameAI &ai)
{
    NodeType *rootNode = ai.getGameTree()->getRoot();

    // test whether root.rating (level0) is equals to the sum of ratings on level2
    // level1 is the opponent's rating
    double sumOfChildRatings = 0.0;
    for (auto &c1 : rootNode->childNodes)
    {
        for (auto &c2 : c1->childNodes)
        {
            sumOfChildRatings += c2->data.rating;
        }
    }
    BOOST_CHECK(rootNode->data.rating == sumOfChildRatings);

    // test whether root.visits are equals to the sum of visits on level1
    int sumOfChildVisits = 0;
    for (auto &c : rootNode->childNodes)
    {
        sumOfChildVisits += c->data.visits;
    }
    BOOST_CHECK(rootNode->data.visits == sumOfChildVisits);
}


// tests tree create, copy assignment and proper deconstruction of a Tree
BOOST_AUTO_TEST_CASE(TreeTest01)
{
    int ACTUAL, EXPECTED;
    BOOST_TEST_MESSAGE("TreeTest01");

    Tree<NodeData> tree = Tree<NodeData>();
    tree.getRoot()->data.sequenceThatLeadedToThisNode = "root1";
    NodeType *newNode0 = tree.createNewNode(tree.getRoot());
    newNode0->data.sequenceThatLeadedToThisNode = "newNode0";
    NodeType *newNode1 = tree.createNewNode(tree.getRoot());
    newNode1->data.sequenceThatLeadedToThisNode = "newNode1";
    NodeType *newNode2 = tree.createNewNode(tree.getRoot());
    newNode2->data.sequenceThatLeadedToThisNode = "newNode2";
    tree.printLevelOrder();
    ACTUAL = tree.size();
    EXPECTED = 4;
    BOOST_CHECK(ACTUAL == EXPECTED);

    const Tree<NodeData> tree2 = tree;
    tree2.getRoot()->data.sequenceThatLeadedToThisNode = "root2";
    Tree<NodeData>::printLevelOrder(tree.getRoot());

    Tree<NodeData> *ptree3 = new Tree<NodeData>;
    ptree3->getRoot()->data.sequenceThatLeadedToThisNode = "root3";
    NodeType *newNode3 = ptree3->createNewNode(ptree3->getRoot());
    newNode3->data.sequenceThatLeadedToThisNode = "newNode3";
    NodeType *newNode4 = ptree3->createNewNode(ptree3->getRoot());
    newNode4->data.sequenceThatLeadedToThisNode = "newNode4";
    NodeType *newNode5 = ptree3->createNewNode(ptree3->getRoot());
    newNode5->data.sequenceThatLeadedToThisNode = "newNode5";
    Tree<NodeData>::printLevelOrder(ptree3->getRoot());
    ACTUAL = tree2.size();
    EXPECTED = 4;
    BOOST_CHECK(ACTUAL == EXPECTED);
    BOOST_CHECK(tree.size() == tree2.size());

    delete ptree3;
}

BOOST_AUTO_TEST_CASE(MCTS_Test01)
{
    BOOST_TEST_MESSAGE("MCTS_Test01");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.insertTokenIntoColumn(5);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(4);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(4);
    gs.insertTokenIntoColumn(3);
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN = 3;
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
} // --run_test=MCTS_Test01

BOOST_AUTO_TEST_CASE(MCTS_Test02)
{
    BOOST_TEST_MESSAGE("MCTS_Test02");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(5);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(4);
    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(2);
    //-> KI has to place in col2 now
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN = 2;
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test03)
{
    BOOST_TEST_MESSAGE("MCTS_Test03");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(0);

    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(1);

    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(4);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(4);

    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(5);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(5);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(6);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(6);

    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(2);
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN = 2;
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test04)
{
    BOOST_TEST_MESSAGE("MCTS_Test04");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(5);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(5);

    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(2);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(2);

    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(2);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(2);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(3);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(6);
    gs.setTurnPlayer(playerAI);
    gs.insertTokenIntoColumn(6);
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN = 3;
    BOOST_CHECK(ACTUAL_COLUMN ==
                EXPECTED_COLUMN); // is it really working always?

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}


BOOST_AUTO_TEST_CASE(MCTS_Test05)
{
    BOOST_TEST_MESSAGE("MCTS_Test05");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerAI); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(4);
    gs.insertTokenIntoColumn(5);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(1);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(2);
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN = 2;
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test06)
{
    BOOST_TEST_MESSAGE("MCTS_Test06");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 7;
    const int MAX_Y = 5;
    GameState gs(MAX_X, MAX_Y);
    gs.setTurnPlayer(playerYou); // set Player that needs to begin here
    GameAI ai(playerAI);

    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(2);
    gs.insertTokenIntoColumn(0);
    gs.insertTokenIntoColumn(5);
    gs.insertTokenIntoColumn(3);
    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(4);
    gs.insertTokenIntoColumn(6);
    gs.insertTokenIntoColumn(5);
    gs.insertTokenIntoColumn(6); // here circle is about to win...
    gs.drawGameStateOnConsole();

    //gs.setTurnPlayer(playerYou);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN =
        6; // should be chosen to avoid circle from winning
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}


BOOST_AUTO_TEST_CASE(MCTS_Test07)
{
    BOOST_TEST_MESSAGE("MCTS_Test07");

    const Player playerYou = Player::PLAYER_2;
    const Player playerAI = Player::PLAYER_1;
    const int MAX_X = 4;
    const int MAX_Y = 4;
    GameState gs(MAX_X, MAX_Y);
    GameAI ai(playerAI);

    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(0);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(1);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(2);
    gs.setTurnPlayer(playerYou);
    gs.insertTokenIntoColumn(0);
    gs.drawGameStateOnConsole();

    gs.setTurnPlayer(playerAI);
    const int ACTUAL_COLUMN = ai.findNextMove(gs);
    const int EXPECTED_COLUMN =
        3; // should be chosen to avoid cross from winning
    BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

    checkChildVisitsValuePlausibily(ai);
    checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

/// tests whether the AI places the first token in the middle of the board
/// on an empty board. there is the highest probabily to win :)
BOOST_AUTO_TEST_CASE(MCTS_Test08)
{
    BOOST_TEST_MESSAGE("MCTS_Test08");

    for (int i = 0; i < 1; ++i)
    {
        const Player playerYou = Player::PLAYER_2;
        const Player playerAI = Player::PLAYER_1;
        const int MAX_X = 7;
        const int MAX_Y = 5;
        GameState gs(MAX_X, MAX_Y);
        GameAI ai(playerAI);

        gs.drawGameStateOnConsole();

        gs.setTurnPlayer(playerAI);
        const int ACTUAL_COLUMN = ai.findNextMove(gs);
        const int EXPECTED_COLUMN = 3; // to avoid cross from winning
        BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

        checkChildVisitsValuePlausibily(ai);
        checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
    }
}
