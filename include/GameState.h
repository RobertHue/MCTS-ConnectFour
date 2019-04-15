#pragma once

#include <vector>
#include <iostream>
#include <stdlib.h>




// see https://doc.qt.io/qt-5/model-view-programming.html#rows-and-columns


const bool NO_VALID_MOVE = false;
const bool VALID_MOVE = true;

enum class Player {	//  underlying type is int (implicit conversions)
    NONE = 0, 
	PLAYER_1 = 1, 
	PLAYER_2 = 2, 
	DRAW = 3	/// used for indicating a TIE
};

struct Position {
	Position() : x(0), y(0) {}
	Position(int x, int y) : x(x), y(y) {}
    int x;
    int y;
	bool Position::operator ==(const Position &b) const {
		if (this->x == b.x && this->y == b.y) { return true; }
		return false;
	}
	bool Position::operator !=(const Position &b) const {
		if (*this == b) { return false; }
		return true;
	}
};

class GameState {
public:
	typedef std::vector<std::vector<int>> GameDataType;
private:
    // smallest size: 4x4
    // biggest size: 8x8
    int MAX_X, MAX_Y;
	GameDataType gameData; /// stores the data needed for the game field

    Player turnPlayer;	/// Player whose turn it is to move
    Player otherPlayer; /// Player who does not move next
    Position positionOfLastPlacedToken;
    int numOfFreeFields;	///num of free fields on the game field

public:
	GameState(int x = 4, int y = 4);	// constructor with arguments

	///////////////////////////////
	//// GETTER  //////
	///////////////////

    int getMAX_X() const;
    int getMAX_Y() const;
    int getNumOfFreeFields() const;

    Player getTurnPlayer() const;
    Player getOtherPlayer() const;

	GameDataType& getGameData() const; // gets a reference to the GameState's GameData
	Position getPositionOfLastPlacedToken() const;
	std::vector<int> getPossibleMoves() const;

	///////////////////////////////
	//// SETTER  //////
	///////////////////

	void setTurnPlayer(Player turnPlayer);

	void nextTurn();

	/** @brief		checks whether the move into column is valid 
	  * if valid,	then this method inserts it into GamePanel and returns VALID_MOVE
	  * otherwhise, returns NO_VALID_MOVE and does nothing
	  */
    bool insertTokenIntoColumn(int column);


	/// test whether turnPlayer would win in column xy; does not alter anything to gamestate; hence the const-qualifier
	Player wouldSomeoneWin(int column) const;

	/** @brief		retun a player, that has won
	 * returns NONE(0), if there is a tie or there is no winner yet 
	 */
    Player hasSomeoneWon();

	///////////////////////////////
	//// VISUALIZE  //////
	//////////////////////

    void drawGameStateOnConsole();
};
