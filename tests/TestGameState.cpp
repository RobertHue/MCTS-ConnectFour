#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "ai/GameAI.h"
#include "ai/Tree.h"
#include "data/GameState.h"

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
