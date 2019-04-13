#define BOOST_TEST_MODULE "TestMCTSModule"
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

#include "GameState.h"
#include "GameAI.h"
#include "Tree.h"



/// tests whether the nodes child node size is <= MAX_X
void checkChildCountIteratively(NodeType * node, const int MAX_X)
{
	if (node == nullptr) { return; }
	
	BOOST_CHECK(node->childNodes.size() <= MAX_X);

	/* first delete all subtrees (from left to right) */
	for (int i = 0; i < node->childNodes.size(); ++i) {
		checkChildCountIteratively(node->childNodes[i], MAX_X);
	}

}


void checkChildVisitsValuePlausibily(const GameAI& ai) {
	NodeType* rootNode = ai.getGameTree()->getRoot();

	// test whether root.rating (level0) is equals to the sum of ratings on level2
	// level1 is the opponent's rating
	double sumOfChildRatings = 0.0;
	for (auto& c1 : rootNode->childNodes) {
		for (auto& c2 : c1->childNodes) {
			sumOfChildRatings += c2->rating;
		}
	}
	BOOST_CHECK(rootNode->rating == sumOfChildRatings);

	// test whether root.visits are equals to the sum of visits on level1
	int sumOfChildVisits = 0;
	for (auto& c : rootNode->childNodes) {
		sumOfChildVisits += c->visits;
	}
	BOOST_CHECK(rootNode->visits == sumOfChildVisits);
}


// tests tree create, copy assignment and proper deconstruction of a Tree
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
		Tree::printLevelOrder(tree.getRoot());
	}

	Tree tree2 = tree;
	Tree::printLevelOrder(tree.getRoot());

	Tree *ptree3 = new Tree;
	{
		NodeType *newNode0 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode0, ptree3->getRoot());
		NodeType *newNode1 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode1, ptree3->getRoot());
		NodeType *newNode2 = ptree3->createNewNode();
		ptree3->addNodeTo(newNode2, ptree3->getRoot());
		Tree::printLevelOrder(ptree3->getRoot());
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

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
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
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 3;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
} // --run_test=MCTS_Test01

BOOST_AUTO_TEST_CASE(MCTS_Test02)
{
	BOOST_TEST_MESSAGE("MCTS_Test02");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
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
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test03)
{
	BOOST_TEST_MESSAGE("MCTS_Test03");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
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
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test04)
{
	BOOST_TEST_MESSAGE("MCTS_Test04");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
	const int MAX_X = 7;
	const int MAX_Y = 5;
	GameState gs(MAX_X, MAX_Y);
	gs.setTurnPlayer(playerAI); // set Player that needs to begin here
	GameAI ai(playerAI);

	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(5);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(5);

	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(0);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(0);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(1);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(1);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(2);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(2);

	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(0);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(1);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(2);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(0);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(1);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(2);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(3);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(6);
	gs.setTurnPlayer(playerAI); gs.insertTokenIntoColumn(6);
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 3;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN); // is it really working always?

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}


BOOST_AUTO_TEST_CASE(MCTS_Test05)
{
	BOOST_TEST_MESSAGE("MCTS_Test05");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
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
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 2;
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

BOOST_AUTO_TEST_CASE(MCTS_Test06)
{
	BOOST_TEST_MESSAGE("MCTS_Test06");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
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
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	//gs.setTurnPlayer(playerYou);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 6;	// should be chosen to avoid circle from winning
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}




BOOST_AUTO_TEST_CASE(MCTS_Test07)
{
	BOOST_TEST_MESSAGE("MCTS_Test07");

	Player playerYou = Player::PLAYER_2;
	Player playerAI = Player::PLAYER_1;
	const int MAX_X = 4;
	const int MAX_Y = 4;
	GameState	gs(MAX_X, MAX_Y);
	GameAI		ai(playerAI);

	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(0);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(1);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(2);
	gs.setTurnPlayer(playerYou); gs.insertTokenIntoColumn(0);
	GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

	gs.setTurnPlayer(playerAI);
	int ACTUAL_COLUMN = ai.findNextMove(gs);
	int EXPECTED_COLUMN = 3;	// should be chosen to avoid cross from winning
	BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

	checkChildVisitsValuePlausibily(ai);
	checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
}

/// tests whether the AI places the first token in the middle of the board
/// on an empty board. there is the highest probabily to win :)
BOOST_AUTO_TEST_CASE(MCTS_Test08)
{
	BOOST_TEST_MESSAGE("MCTS_Test08");

	for (int i = 0; i < 1; ++i) {
		Player playerYou = Player::PLAYER_2;
		Player playerAI = Player::PLAYER_1;
		const int MAX_X = 7;
		const int MAX_Y = 5;
		GameState	gs(MAX_X, MAX_Y);
		GameAI		ai(playerAI);

		GameState::drawGameStateOnConsole(gs.getGameData(), gs.getMAX_X(), gs.getMAX_Y());

		gs.setTurnPlayer(playerAI);
		int ACTUAL_COLUMN = ai.findNextMove(gs);
		int EXPECTED_COLUMN = 3;	// should be chosen to avoid cross from winning
		BOOST_CHECK(ACTUAL_COLUMN == EXPECTED_COLUMN);

		checkChildVisitsValuePlausibily(ai);
		checkChildCountIteratively(ai.getGameTree()->getRoot(), gs.getMAX_X());
	}
}