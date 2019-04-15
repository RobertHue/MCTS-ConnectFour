
#include "GameState.h"

///////////////////////////////////////////////

GameState::GameState(int x, int y) : MAX_X(x), MAX_Y(y)
{
	gameData = GameState::GameDataType(MAX_X, std::vector<int>(MAX_Y, static_cast<int>(Player::NONE)));

	this->setTurnPlayer(Player::PLAYER_1);
	this->numOfFreeFields = MAX_X * MAX_Y;
	// hier evtl noch abprfen ob spielfeld gerade anzahl an feldern hat; d.h. fair ist...
}

///////////////////////////////
//// GETTER  //////
///////////////////

int GameState::getMAX_X() const {
	return MAX_X;
}

int GameState::getMAX_Y() const {
	return MAX_Y;
}

int GameState::getNumOfFreeFields() const {
    return this->numOfFreeFields;
}

////////////////////////////////////////////

Player GameState::getTurnPlayer() const {
	return this->turnPlayer;
}

Player GameState::getOtherPlayer() const {
	return this->otherPlayer;
}

////////////////////////////////////////////

GameState::GameDataType GameState::getGameData() const {
	return gameData;
}

Position GameState::getPositionOfLastPlacedToken() const {
    return this->positionOfLastPlacedToken;
}

std::vector<int> GameState::getPossibleMoves() const
{
	// stores the cols which can be made as possible moves
	std::vector<int> possibleMoves;

	int row, col;
	for (col = 0; col < MAX_X; ++col) {
		for (row = MAX_Y - 1; row >= 0; --row) {

			// check whether insertion cell is still free
			if (gameData[col][row] == static_cast<int>(Player::NONE)) {
				possibleMoves.push_back(col);
				break;	// break out of rows loop
			}
		}
	}

	return possibleMoves;
}

///////////////////////////////
//// SETTER  //////
///////////////////

void GameState::setTurnPlayer(Player turnPlayer) {
	// protection against wrong parameters:
	if (turnPlayer != Player::PLAYER_1 &&
		turnPlayer != Player::PLAYER_2) {
		return;
	}

	// set the turnPlayer and assign opposite player to otherPlayer
    this->turnPlayer = turnPlayer;
	this->otherPlayer = (this->turnPlayer == Player::PLAYER_1) ? 
		Player::PLAYER_2 : Player::PLAYER_1;
}


void GameState::nextTurn() {
    if (turnPlayer == Player::PLAYER_1) {
        turnPlayer = Player::PLAYER_2;
        otherPlayer = Player::PLAYER_1;
    } else {
        turnPlayer = Player::PLAYER_1;
        otherPlayer = Player::PLAYER_2;
    }
}

bool GameState::insertTokenIntoColumn(int column) {
	int row;
	for (row = MAX_Y - 1; row >= 0; --row) {

		// check whether insertion cell is still free
		if (gameData[column][row] == static_cast<int>(Player::NONE)) {
			// insert token into the cell, which is free:
			gameData[column][row] = static_cast<int>(this->turnPlayer);

			// remember the position of the last placed token
			positionOfLastPlacedToken.x = column;
			positionOfLastPlacedToken.y = row;

			// decrease the number of left free fields on the GamePanel
			--numOfFreeFields;

			// swap TurnPlayer, in case of an valid move
			nextTurn();
			return VALID_MOVE;
		}
	}
	// token couldn't be placed at column. Please choose again!
	// Reason: Column is already full
	return NO_VALID_MOVE;
}

Player GameState::hasSomeoneWon() {
    int x_placed = positionOfLastPlacedToken.x;
    int y_placed = positionOfLastPlacedToken.y;

    // count how many tokens there are in each row:
    int num_of_tokens_p1_in_row = 0;
    int num_of_tokens_p2_in_row = 0;

    int offsets[][2] = {
        { -3, 0},
        { -2, 0},
        { -1, 0},
        { 0, 0},
        { 1, 0},
        { 2, 0},
        { 3, 0}, // horizontal
        { 0, -3},
        { 0, -2},
        { 0, -1},
        { 0, 0},
        { 0, 1},
        { 0, 2},
        { 0, 3}, // vertical
        { -3, -3},
        { -2, -2},
        { -1, -1},
        { 0, 0},
        { 1, 1},
        { 2, 2},
        { 3, 3}, // diagonal1
        { -3, 3},
        { -2, 2},
        { -1, 1},
        { 0, 0},
        { 1, -1},
        { 2, -2},
        { 3, -3} // diagonal2
    };
    // cout << sizeof(offsets) << endl;

	// check all 8 directions on the current placed token, and count the number of tokens in each of those directions
    for (size_t i = 0; i < ((sizeof (offsets) / sizeof (int)) / 2); ++i) {
		// new line inside the offset_Matrix => new check of number of tokens in each of the 8 directions
        if ((i % 7) == 0) {
            num_of_tokens_p1_in_row = 0;
            num_of_tokens_p2_in_row = 0;
        }
        int x_check = x_placed + offsets[i][0];
        int y_check = y_placed + offsets[i][1];

        // check whether bounds are reached in those directions:
        if (x_check < 0 || x_check >= MAX_X) continue;
        if (y_check < 0 || y_check >= MAX_Y) continue;


        if (this->gameData[x_check][y_check] == static_cast<int>(Player::PLAYER_1)) {
            num_of_tokens_p2_in_row = 0;
            ++num_of_tokens_p1_in_row;
        } else if (this->gameData[x_check][y_check] == static_cast<int>(Player::PLAYER_2)) {
            num_of_tokens_p1_in_row = 0;
            ++num_of_tokens_p2_in_row;
        } else if (this->gameData[x_check][y_check] == static_cast<int>(Player::NONE)) {
            num_of_tokens_p1_in_row = 0;
            num_of_tokens_p2_in_row = 0;
        }

        // has someone won?
        if (num_of_tokens_p1_in_row >= 4) return Player::PLAYER_1;
        else if (num_of_tokens_p2_in_row >= 4) return Player::PLAYER_2;
    }

	// if number of free fields are 0,
	if (numOfFreeFields == 0) {
		// then there is a tie!
		return Player::DRAW;
	}

    return Player::NONE; // noone has won yet!
}

///////////////////////////////
//// VISUALIZE  //////
//////////////////////

void GameState::drawGameStateOnConsole(GameState::GameDataType gameData, int MAX_X, int MAX_Y) {
    // system("cls");
	std::cout << "Connect-Four (" << MAX_Y << "x" << MAX_X << "):\n\n\n";
	std::cout << "  ";
    for (int x = 0; x < MAX_X; ++x) {
		std::cout << " " << x;
    }
	std::cout << "\n  ";


    for (int x = 0; x < MAX_X; ++x) {
		std::cout << "--";
    }
	std::cout << "-\n";


    for (int y = 0; y < MAX_Y; ++y) {
		std::cout << y << " ";
        for (int x = 0; x < MAX_X; ++x) {
			std::cout << "|";

            switch (gameData[x][y]) {
                case static_cast<int>(Player::NONE) :
					std::cout << " "; // freier Platz
                    break;
                case static_cast<int>(Player::PLAYER_1):
					std::cout << "O";
                    break;
                case static_cast<int>(Player::PLAYER_2):
					std::cout << "X";
                    break;
                default:
					std::cout << "E"; // Error...
            }
        }
        // zwischenzeile fr Trennlinie
		std::cout << "|\n  ";
        for (int x = 0; x < MAX_X; ++x) {
			std::cout << "--";
        }
		std::cout << "-\n";
    }
	std::cout << "\n";
}