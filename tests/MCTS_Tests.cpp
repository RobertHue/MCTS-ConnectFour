#define BOOST_TEST_MODULE "TestMCTSModule"
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

#include "GamePanel.h"
#include "GameAI.h"
#include "Tree.h"

BOOST_AUTO_TEST_CASE(TreeTest01)
{
	int ACTUAL, EXPECTED;
	BOOST_TEST_MESSAGE("TreeTest01");

	Tree tree = Tree();

	{
		NodeType *newNode0 = tree.createNewNode();
		tree.addNodeTo(newNode0, tree.getRoot());
		NodeType *newNode1 = tree.createNewNode();
		tree.addNodeTo(newNode1, tree.getRoot());
		NodeType *newNode2 = tree.createNewNode();
		tree.addNodeTo(newNode2, tree.getRoot());
		tree.levelOrder(tree.getRoot());
	}

	Tree tree2 = tree;
	tree.levelOrder(tree.getRoot());

	Tree *ptree3 = new Tree;
	{
		NodeType *newNode0 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode0, ptree3->getRoot());
		NodeType *newNode1 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode1, ptree3->getRoot());
		NodeType *newNode2 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode2, ptree3->getRoot());
		ptree3->levelOrder(ptree3->getRoot());
	}

	ACTUAL = tree.getAmountOfNodes();
	EXPECTED = 4;
	BOOST_CHECK(ACTUAL == EXPECTED);

	ACTUAL = tree2.getAmountOfNodes();
	EXPECTED = 4;
	BOOST_CHECK(ACTUAL == EXPECTED);

	BOOST_CHECK(tree.getAmountOfNodes() == tree2.getAmountOfNodes());
	delete ptree3;
}


BOOST_AUTO_TEST_CASE( MCTS_Test01 )
{
	BOOST_TEST_MESSAGE("MCTS_Test01");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);

	gp.insertTokenIntoColumn(5);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(4);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(4);
	gp.insertTokenIntoColumn(3);
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	gp.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = gKI.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 3;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
}

BOOST_AUTO_TEST_CASE(MCTS_Test02)
{
	BOOST_TEST_MESSAGE("MCTS_Test02");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);

	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(5);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(4);
	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(2);
	//-> KI has to place in col2 now
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	gp.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = gKI.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
}

BOOST_AUTO_TEST_CASE(MCTS_Test03)
{
	BOOST_TEST_MESSAGE("MCTS_Test03");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);

	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(0);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(0);

	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(1);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(1);

	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(4);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(4);

	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(5);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(5);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(6);
	gp.setTurnPlayer(playerAI);
	gp.insertTokenIntoColumn(6);

	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(2);
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	gp.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = gKI.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
}

BOOST_AUTO_TEST_CASE(MCTS_Test04)
{
	BOOST_TEST_MESSAGE("MCTS_Test04");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);

	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(5);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(5);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(5);


	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(0);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(0);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(1);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(1);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(2);
	gp.setTurnPlayer(playerYou); gp.insertTokenIntoColumn(2);

	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(0);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(1);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(2);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(0);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(1);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(2);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(3);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(6);
	gp.setTurnPlayer(playerAI); gp.insertTokenIntoColumn(6);
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	gp.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = gKI.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 5;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
}


BOOST_AUTO_TEST_CASE(MCTS_Test05)
{
	BOOST_TEST_MESSAGE("MCTS_Test05");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);

	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(4);
	gp.insertTokenIntoColumn(5);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(1);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(2);
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	gp.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = gKI.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
}

BOOST_AUTO_TEST_CASE(MCTS_Test06)
{
	BOOST_TEST_CHECKPOINT("Starting MCTS_Test06");
	BOOST_TEST_MESSAGE("MCTS_Test06");

	Player playerYou = PLAYER_2;
	Player playerAI = PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GamePanel gp(MAX_X, MAX_Y);
	gp.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI gKI(playerAI);
	GameAI gKI2(playerYou);

	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(2);
	gp.insertTokenIntoColumn(0);
	gp.insertTokenIntoColumn(5);
	gp.insertTokenIntoColumn(3);
	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(4);
	gp.insertTokenIntoColumn(6);
	gp.insertTokenIntoColumn(5);
	gp.insertTokenIntoColumn(6); // here circle is about to win...
	GamePanel::drawGamePanelOnConsole(gp.getGameData(), gp.getMAX_X(), gp.getMAX_Y());

	//gp.setTurnPlayer(playerYou);
	int ACTUAL_COLUMN = gKI2.calculateNextTurn(gp);
	int EXPECTED_COLUMN = 6;	// should be chosen to avoid circle from winning
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);
	BOOST_TEST_CHECKPOINT("Ending MCTS_Test06");
}