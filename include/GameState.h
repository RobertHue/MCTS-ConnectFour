#pragma once

#include <vector>
#include <iostream>
#include <stdlib.h>

#define FREE_FIELD 0


// see https://doc.qt.io/qt-5/model-view-programming.html#rows-and-columns


enum Player {
    NONE = 0, 
	PLAYER_1 = 1, 
	PLAYER_2 = 2, 
	BOTH = 3	/// used for indicating a TIE
};

const int NO_VALID_MOVE = -1;
const int VALID_MOVE = 0;

typedef struct {
    int x;
    int y;
} Position;

class GameState {
private:
    // smallest size: 4x4
    // biggest size: 8x8
    int MAX_X, MAX_Y;
    std::vector<std::vector<int>> gameData; /// stores the data needed for the game field

    Player turnPlayer;	/// Player whose turn it is to move
    Player otherPlayer; /// Player who does not move next
    Position positionOfLastPlacedToken;
    int numOfFreeFields;	///num of free fields on the game field

public:

    GameState(int x = 4, int y = 4) : MAX_X(x), MAX_Y(y) {
        gameData = std::vector<std::vector<int>> (MAX_X, std::vector<int> (MAX_Y, FREE_FIELD));

        this->setTurnPlayer(PLAYER_1);
        this->numOfFreeFields = MAX_X * MAX_Y;
        // hier evtl noch abprfen ob spielfeld gerade anzahl an feldern hat; d.h. fair ist...
    }
    void setTurnPlayer(Player turnPlayer); 

    Position getPositionOfLastPlacedToken() const;
    int getMAX_X() const;
    int getMAX_Y() const;
    int getNumOfFreeFields() const;
    Player getTurnPlayer() const;
    Player getOtherPlayer() const;
    std::vector<std::vector<int>> getGameData() const;

	/** @brief		checks whether the move into column is valid 
	  * if valid,	then this method inserts it into GamePanel and returns VALID_MOVE
	  * otherwhise, returns NO_VALID_MOVE and does nothing
	  */
    int insertTokenIntoColumn(int column);

	/** @brief		retun a player, that has won
	 * returns NONE(0), if there is a tie or there is no winner yet 
	 */
    Player hasSomeoneWon();

    Player isAboutToWin();
    void nextTurn();

    static void drawGameStateOnConsole(std::vector<std::vector<int>> gameData, int max_x, int max_y);
};
