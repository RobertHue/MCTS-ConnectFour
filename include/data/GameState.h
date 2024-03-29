﻿#pragma once

#include <iostream>
#include <stdlib.h>
#include <vector>


const bool NO_VALID_MOVE = false;
const bool VALID_MOVE = true;

enum class Player
{ //  underlying type is int (implicit conversions)
    NONE = 0,
    PLAYER_1 = 1,
    PLAYER_2 = 2,
    DRAW = 3 /// used for indicating a TIE
};

struct Position
{
    Position() : x(0), y(0)
    {
    }
    Position(int x, int y) : x(x), y(y)
    {
    }
    int x;
    int y;
    bool operator==(const Position &b) const
    {
        if (this->x == b.x && this->y == b.y)
        {
            return true;
        }
        return false;
    }
    bool operator!=(const Position &b) const
    {
        if (*this == b)
        {
            return false;
        }
        return true;
    }
};

class GameState
{
public:
    typedef std::vector<std::vector<int>> GameDataType;

private:
    // smallest size: 4x4
    // biggest size: 8x8
    int MAX_X;               /// the number of cols
    int MAX_Y;               /// the number of rows
    GameDataType m_gameData; /// stores the data needed for the game field

    Player turnPlayer;       /// Player whose turn it is to move
    Player otherPlayer;      /// Player who does not move next
    Position positionOfLastPlacedToken;
    int numOfFreeFields;     ///num of free fields on the game field

public:
    GameState(int x = 4, int y = 4); // constructor with arguments

    ///////////////////
    //// GETTER  //////
    ///////////////////

    int getMAX_X() const; /// returns the number of cols
    int getMAX_Y() const; /// returns the number of rows
    int getNumOfFreeFields() const;

    int getGameDataAt(int x, int y) const;

    Player getTurnPlayer() const;
    Player getOtherPlayer() const;

    const GameDataType &getGameData()
        const; // gets a const reference to the GameState's GameData
    Position getPositionOfLastPlacedToken() const;
    std::vector<int> getPossibleMoves() const;

    ///////////////////
    //// SETTER  //////
    ///////////////////

    void setTurnPlayer(Player turnPlayer);

    void nextTurn();

    /// @brief Checks whether the move into column is valid
    /// @return if valid, then this method inserts it into GamePanel and returns VALID_MOVE
    /// otherwhise, returns NO_VALID_MOVE and does nothing
    bool insertTokenIntoColumn(int column);
    // TODO Should follow SOLID principles (desc says checks but it does more than checking; here also inserting)

    /// @brief Tests whether turnPlayer would win in column xy; does not alter anything to gamestate; hence the const-qualifier
    Player wouldSomeoneWin(int column) const;

    /// @brief Checks whether some player has won
    /// @return NONE(0), if there is a tie or there is no winner yet
    Player hasSomeoneWon();

    //////////////////////
    //// VISUALIZE  //////
    //////////////////////

    void drawGameStateOnConsole();
};
