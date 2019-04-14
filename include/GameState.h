#pragma once

#include <vector>
#include <iostream>
#include <stdlib.h>



// see https://doc.qt.io/qt-5/model-view-programming.html#rows-and-columns


enum class Player {	//  underlying type is int (implicit conversions)
    NONE = 0, 
	PLAYER_1 = 1, 
	PLAYER_2 = 2, 
	DRAW = 3	/// used for indicating a TIE
};


const bool NO_VALID_MOVE	= false;
const bool VALID_MOVE		= true;

typedef struct {
    int x;
    int y;
} Position;

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

	GameDataType getGameData() const;
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

	/** @brief		retun a player, that has won
	 * returns NONE(0), if there is a tie or there is no winner yet 
	 */
    Player hasSomeoneWon();

	///////////////////////////////
	//// VISUALIZE  //////
	//////////////////////

    static void drawGameStateOnConsole(GameDataType gameData, int max_x, int max_y);
};
