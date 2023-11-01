#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "ai/GameAI.h"
#include "ai/Tree.h"
#include "data/GameState.h"

BOOST_AUTO_TEST_CASE(TreeTest00)
{
    BOOST_TEST_MESSAGE("TreeTest00");

    const Tree<int> tree;

    //BOOST_CHECK(tree.size() == tree2.size());
}


/// tests whether the nodes child node size is <= MAX_X
void checkChildCountIteratively(NodeTypePtr node, const int MAX_X)
{
    if (node == nullptr)
    {
        return;
    }

    BOOST_CHECK(node->childNodes.size() <= static_cast<std::size_t>(MAX_X));

    /* first delete all subtrees (from left to right) */
    for (const auto &child : node->childNodes)
    {
        checkChildCountIteratively(child, MAX_X);
    }
}


void checkChildVisitsValuePlausibily(const GameAI &ai)
{
    const NodeTypePtr rootNode = ai.getGameTree().getRoot();

    // test whether root.rating (level0) is equals to the sum of ratings on level2
    // level1 is the opponent's rating
    double sumOfChildRatings = 0.0;
    for (auto &child_prev : rootNode->childNodes)
    {
        for (auto &child_next : child_prev->childNodes)
        {
            sumOfChildRatings += child_next->data.rating;
        }
    }
    BOOST_CHECK(rootNode->data.rating == sumOfChildRatings);

    // test whether root.visits are equals to the sum of visits on level1
    std::size_t sumOfChildVisits = 0;
    for (auto &child : rootNode->childNodes)
    {
        sumOfChildVisits += child->data.visits;
    }
    BOOST_CHECK(rootNode->data.visits == sumOfChildVisits);
}


// tests tree create, copy assignment and proper deconstruction of a Tree
BOOST_AUTO_TEST_CASE(TreeTest01)
{
    std::size_t ACTUAL = 0, EXPECTED = 0;
    BOOST_TEST_MESSAGE("TreeTest01");

    Tree<NodeData> tree = Tree<NodeData>();
    tree.getRoot()->data.sequenceThatLeadedToThisNode = "root1";
    const NodeTypePtr newNode0 = tree.createNewNode(tree.getRoot());
    newNode0->data.sequenceThatLeadedToThisNode = "newNode0";
    const NodeTypePtr newNode1 = tree.createNewNode(tree.getRoot());
    newNode1->data.sequenceThatLeadedToThisNode = "newNode1";
    const NodeTypePtr newNode2 = tree.createNewNode(tree.getRoot());
    newNode2->data.sequenceThatLeadedToThisNode = "newNode2";
    tree.printLevelOrder();
    ACTUAL = tree.size();
    EXPECTED = 4;
    BOOST_CHECK(ACTUAL == EXPECTED);

    const Tree<NodeData> tree2 = tree;
    tree2.getRoot()->data.sequenceThatLeadedToThisNode = "root2";
    Tree<NodeData>::printLevelOrder(tree.getRoot());

    auto ptree3 = std::make_unique<Tree<NodeData>>();
    ptree3->getRoot()->data.sequenceThatLeadedToThisNode = "root3";
    const NodeTypePtr newNode3 = ptree3->createNewNode(ptree3->getRoot());
    newNode3->data.sequenceThatLeadedToThisNode = "newNode3";
    const NodeTypePtr newNode4 = ptree3->createNewNode(ptree3->getRoot());
    newNode4->data.sequenceThatLeadedToThisNode = "newNode4";
    const NodeTypePtr newNode5 = ptree3->createNewNode(ptree3->getRoot());
    newNode5->data.sequenceThatLeadedToThisNode = "newNode5";
    Tree<NodeData>::printLevelOrder(ptree3->getRoot());
    ACTUAL = tree2.size();
    EXPECTED = 4;
    BOOST_CHECK(ACTUAL == EXPECTED);
    BOOST_CHECK(tree.size() == tree2.size());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
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
    checkChildCountIteratively(ai.getGameTree().getRoot(), gs.getMAX_X());
}
